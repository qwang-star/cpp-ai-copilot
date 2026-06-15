#include "copilot/simple_server.hpp"

#include <array>
#include <chrono>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <string>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

namespace copilot {
namespace {

#ifdef _WIN32
using SocketHandle = SOCKET;
constexpr SocketHandle invalid_socket_handle = INVALID_SOCKET;

void close_socket(SocketHandle socket) {
    closesocket(socket);
}

class WinsockSession {
public:
    WinsockSession() {
        WSADATA data{};
        if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
            throw std::runtime_error("WSAStartup failed");
        }
    }

    ~WinsockSession() {
        WSACleanup();
    }
};
#else
using SocketHandle = int;
constexpr SocketHandle invalid_socket_handle = -1;

void close_socket(SocketHandle socket) {
    close(socket);
}

class WinsockSession {
public:
    WinsockSession() = default;
};
#endif

std::string socket_error_message(const std::string& action) {
    std::ostringstream output;
    output << action << " failed";
    return output.str();
}

}  // namespace

SimpleHttpServer::SimpleHttpServer(AppConfig config, Router router, Logger logger)
    : config_(std::move(config)), router_(std::move(router)), logger_(std::move(logger)) {}

int SimpleHttpServer::run() {
    WinsockSession winsock;

    SocketHandle server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == invalid_socket_handle) {
        throw std::runtime_error(socket_error_message("socket"));
    }

    int reuse = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&reuse), sizeof(reuse));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port = htons(static_cast<unsigned short>(config_.port));
    if (inet_pton(AF_INET, config_.host.c_str(), &address.sin_addr) != 1) {
        close_socket(server_socket);
        throw std::runtime_error("APP_HOST must be an IPv4 address");
    }

    if (bind(server_socket, reinterpret_cast<sockaddr*>(&address), sizeof(address)) != 0) {
        close_socket(server_socket);
        throw std::runtime_error(socket_error_message("bind"));
    }

    if (listen(server_socket, SOMAXCONN) != 0) {
        close_socket(server_socket);
        throw std::runtime_error(socket_error_message("listen"));
    }

    std::ostringstream startup;
    startup << "cpp-ai-copilot listening on http://" << config_.host << ':' << config_.port;
    logger_.info(startup.str());

    while (true) {
        sockaddr_in client_address{};
#ifdef _WIN32
        int client_length = sizeof(client_address);
#else
        socklen_t client_length = sizeof(client_address);
#endif
        SocketHandle client_socket =
            accept(server_socket, reinterpret_cast<sockaddr*>(&client_address), &client_length);
        if (client_socket == invalid_socket_handle) {
            logger_.warn(socket_error_message("accept"));
            continue;
        }

        std::array<char, 8192> buffer{};
        const auto start = std::chrono::steady_clock::now();
        const int received = recv(client_socket, buffer.data(), static_cast<int>(buffer.size() - 1), 0);
        if (received <= 0) {
            close_socket(client_socket);
            continue;
        }

        HttpRequest request = parse_http_request(std::string(buffer.data(), static_cast<std::size_t>(received)));
        HttpResponse response;
        try {
            response = router_.route(request);
        } catch (const std::exception& error) {  //exception是所有标准异常的基类，捕获它可以处理所有类型的标准异常
            logger_.error(error.what());
            response = HttpResponse::json(500, R"({"error":"internal_server_error"})");
        }

        const std::string serialized = response.serialize();
        send(client_socket, serialized.data(), static_cast<int>(serialized.size()), 0);
        close_socket(client_socket);

        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start);
        std::ostringstream access;
        access << request.method << ' ' << request.path << " -> " << response.status_code << " in "
               << elapsed.count() << "ms";
        logger_.info(access.str());
    }
}

}  // namespace copilot

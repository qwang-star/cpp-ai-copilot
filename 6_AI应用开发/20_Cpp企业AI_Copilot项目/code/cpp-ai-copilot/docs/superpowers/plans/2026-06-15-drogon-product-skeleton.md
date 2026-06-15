# Drogon Product Skeleton Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add a Drogon-based product server with unified JSON responses, health checking, error handling, request logging, configuration, and tests while preserving the existing teaching server.

**Architecture:** Drogon v1.9.13 is vendored as a pinned Git submodule checkout under `third_party/drogon` and built by the existing top-level CMake project. Framework-independent error metadata and response builders live under `copilot::product`; a Drogon `HttpSimpleController` owns `/health`, while the product entry point owns framework-wide 404, 405, exception, configuration, and access-log behavior.

**Tech Stack:** C++20, CMake 3.20+, Ninja, Drogon v1.9.13, JsonCpp through Drogon, MSYS2 UCRT64 GCC

---

## File Map

| Path | Responsibility |
|---|---|
| `third_party/drogon/` | Pinned Drogon v1.9.13 source and its Trantor submodule |
| `include/copilot/product/error_code.hpp` | Stable project error codes, messages, and HTTP mappings |
| `include/copilot/product/api_response.hpp` | Unified JSON and Drogon response declarations |
| `src/product/error_code.cpp` | Error metadata implementation |
| `src/product/api_response.cpp` | Unified JSON body and `HttpResponse` construction |
| `src/product/health_controller.hpp` | `/health` route declaration |
| `src/product/health_controller.cpp` | Health response implementation |
| `src/product/main.cpp` | Product startup, config, global errors, exceptions, and access logs |
| `config/product.json` | Drogon listener, log, and custom application configuration |
| `tests/test_api_response.cpp` | Unit tests for response shape and error mappings |
| `CMakeLists.txt` | Legacy, product, and test build targets |
| `Makefile` | Existing teaching workflow plus CMake product convenience commands |
| `README.md` | Build and run instructions for both server variants |
| `03_从教学HTTP服务器升级成熟Web框架过程记录.md` | Dependency versions, commands, results, and beginner-oriented explanation |

### Task 1: Acquire and Pin Drogon v1.9.13

**Files:**
- Populate: `third_party/drogon/`
- Verify: `third_party/drogon/CMakeLists.txt`
- Verify: `third_party/drogon/trantor/CMakeLists.txt`

- [ ] **Step 1: Verify the dependency directory is safe to initialize**

Run:

```powershell
Get-ChildItem -Force third_party\drogon
```

Expected: no files. If files exist, inspect them before proceeding and do not overwrite them.

- [ ] **Step 2: Initialize the existing empty directory as a Git repository**

Run:

```powershell
git -C third_party\drogon init
git -C third_party\drogon remote add origin https://github.com/drogonframework/drogon.git
```

Expected: Git repository initialized and remote `origin` added.

- [ ] **Step 3: Fetch the pinned release**

Run:

```powershell
git -C third_party\drogon -c http.version=HTTP/1.1 fetch --depth 1 origin tag v1.9.13
git -C third_party\drogon checkout --detach FETCH_HEAD
```

Expected: checkout succeeds at tag `v1.9.13`.

- [ ] **Step 4: Fetch required submodules**

Run:

```powershell
git -C third_party\drogon submodule update --init --recursive --depth 1
```

Expected: `third_party/drogon/trantor/CMakeLists.txt` exists.

- [ ] **Step 5: Record the exact dependency identity**

Run:

```powershell
git -C third_party\drogon describe --tags --always
git -C third_party\drogon rev-parse HEAD
git -C third_party\drogon submodule status
```

Expected: first line reports `v1.9.13`; commit and Trantor identities are retained for the process document.

- [ ] **Step 6: Verify the existing teaching tests before product changes**

Run:

```powershell
mingw32-make test
```

Expected: `build/test_core.exe` exits successfully.

- [ ] **Step 7: Commit dependency metadata when Git becomes available**

This workspace currently has no parent Git repository. Do not create an unrelated repository automatically. If the project is later placed under Git, commit the pinned dependency before continuing:

```powershell
git add third_party/drogon
git commit -m "build: pin Drogon 1.9.13"
```

### Task 2: Add Error Codes with a Failing Unit Test

**Files:**
- Create: `include/copilot/product/error_code.hpp`
- Create: `src/product/error_code.cpp`
- Create: `tests/test_api_response.cpp`
- Modify: `CMakeLists.txt`

- [ ] **Step 1: Write the failing error-code test**

Create `tests/test_api_response.cpp`:

```cpp
#include "copilot/product/api_response.hpp"
#include "copilot/product/error_code.hpp"

#include <cassert>
#include <string>

using copilot::product::ErrorCode;

void test_error_metadata() {
    using copilot::product::error_message;
    using copilot::product::http_status;
    using copilot::product::to_string;

    assert(std::string(to_string(ErrorCode::route_not_found)) == "ROUTE_NOT_FOUND");
    assert(std::string(error_message(ErrorCode::route_not_found)) == "route not found");
    assert(http_status(ErrorCode::route_not_found) == drogon::k404NotFound);
    assert(http_status(ErrorCode::method_not_allowed) == drogon::k405MethodNotAllowed);
    assert(http_status(ErrorCode::internal_error) == drogon::k500InternalServerError);
}

int main() {
    test_error_metadata();
}
```

- [ ] **Step 2: Add the minimum Drogon and failing-test targets**

Add these options before the existing `copilot_core` target in `CMakeLists.txt`:

```cmake
set(BUILD_CTL OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_ORM OFF CACHE BOOL "" FORCE)
set(BUILD_BROTLI OFF CACHE BOOL "" FORCE)
set(BUILD_YAML_CONFIG OFF CACHE BOOL "" FORCE)
set(USE_SUBMODULE ON CACHE BOOL "" FORCE)

add_subdirectory(third_party/drogon)
```

Append this initial test target after the existing `test_core` target:

```cmake
add_executable(test_api_response
    tests/test_api_response.cpp
)

target_include_directories(test_api_response PRIVATE include)
target_link_libraries(test_api_response PRIVATE Drogon::Drogon)

add_test(NAME api_response COMMAND test_api_response)
```

- [ ] **Step 3: Configure and verify the test fails**

Run:

```powershell
E:\Software\MySY2\ucrt64\bin\cmake.exe -S . -B build-product -G Ninja -DCMAKE_BUILD_TYPE=Debug
E:\Software\MySY2\ucrt64\bin\cmake.exe --build build-product --target test_api_response
```

Expected: compilation fails because `copilot/product/api_response.hpp` and `error_code.hpp` do not exist.

- [ ] **Step 4: Declare the error-code API**

Create `include/copilot/product/error_code.hpp`:

```cpp
#pragma once

#include <drogon/HttpTypes.h>

namespace copilot::product {

enum class ErrorCode {
    ok,
    invalid_request,
    route_not_found,
    method_not_allowed,
    internal_error,
};

const char* to_string(ErrorCode code);
const char* error_message(ErrorCode code);
drogon::HttpStatusCode http_status(ErrorCode code);

}  // namespace copilot::product
```

- [ ] **Step 5: Implement complete error metadata mappings**

Create `src/product/error_code.cpp`:

```cpp
#include "copilot/product/error_code.hpp"

namespace copilot::product {

const char* to_string(ErrorCode code) {
    switch (code) {
        case ErrorCode::ok:
            return "OK";
        case ErrorCode::invalid_request:
            return "INVALID_REQUEST";
        case ErrorCode::route_not_found:
            return "ROUTE_NOT_FOUND";
        case ErrorCode::method_not_allowed:
            return "METHOD_NOT_ALLOWED";
        case ErrorCode::internal_error:
            return "INTERNAL_ERROR";
    }
    return "INTERNAL_ERROR";
}

const char* error_message(ErrorCode code) {
    switch (code) {
        case ErrorCode::ok:
            return "success";
        case ErrorCode::invalid_request:
            return "invalid request";
        case ErrorCode::route_not_found:
            return "route not found";
        case ErrorCode::method_not_allowed:
            return "method not allowed";
        case ErrorCode::internal_error:
            return "internal server error";
    }
    return "internal server error";
}

drogon::HttpStatusCode http_status(ErrorCode code) {
    switch (code) {
        case ErrorCode::ok:
            return drogon::k200OK;
        case ErrorCode::invalid_request:
            return drogon::k400BadRequest;
        case ErrorCode::route_not_found:
            return drogon::k404NotFound;
        case ErrorCode::method_not_allowed:
            return drogon::k405MethodNotAllowed;
        case ErrorCode::internal_error:
            return drogon::k500InternalServerError;
    }
    return drogon::k500InternalServerError;
}

}  // namespace copilot::product
```

- [ ] **Step 6: Add the temporary response declaration required by the test include**

Create `include/copilot/product/api_response.hpp`:

```cpp
#pragma once

#include "copilot/product/error_code.hpp"

#include <drogon/HttpResponse.h>
#include <json/json.h>
#include <string>

namespace copilot::product {

Json::Value success_body(Json::Value data);
Json::Value error_body(ErrorCode code);
drogon::HttpResponsePtr json_response(
    drogon::HttpStatusCode status,
    const Json::Value& body);
drogon::HttpResponsePtr success_response(Json::Value data);
drogon::HttpResponsePtr error_response(ErrorCode code);

}  // namespace copilot::product
```

Create `src/product/api_response.cpp` with definitions that intentionally throw until Task 3:

```cpp
#include "copilot/product/api_response.hpp"

#include <stdexcept>

namespace copilot::product {

Json::Value success_body(Json::Value) {
    throw std::logic_error("success_body is not implemented");
}

Json::Value error_body(ErrorCode) {
    throw std::logic_error("error_body is not implemented");
}

drogon::HttpResponsePtr json_response(
    drogon::HttpStatusCode,
    const Json::Value&) {
    throw std::logic_error("json_response is not implemented");
}

drogon::HttpResponsePtr success_response(Json::Value) {
    throw std::logic_error("success_response is not implemented");
}

drogon::HttpResponsePtr error_response(ErrorCode) {
    throw std::logic_error("error_response is not implemented");
}

}  // namespace copilot::product
```

- [ ] **Step 7: Build and run the focused test**

Add the implementation files to the test target:

```cmake
target_sources(test_api_response PRIVATE
    src/product/error_code.cpp
    src/product/api_response.cpp
)
```

Run:

```powershell
E:\Software\MySY2\ucrt64\bin\cmake.exe --build build-product --target test_api_response
.\build-product\test_api_response.exe
```

Expected: build succeeds and the current metadata-only test passes.

- [ ] **Step 8: Commit when Git becomes available**

```powershell
git add include/copilot/product tests/test_api_response.cpp src/product/error_code.cpp src/product/api_response.cpp CMakeLists.txt
git commit -m "feat: define product API error codes"
```

### Task 3: Implement Unified JSON Responses Test-First

**Files:**
- Modify: `tests/test_api_response.cpp`
- Modify: `src/product/api_response.cpp`

- [ ] **Step 1: Add failing response-body tests**

Replace `main()` and add these functions in `tests/test_api_response.cpp`:

```cpp
void test_success_body() {
    Json::Value data;
    data["status"] = "ok";

    const Json::Value body = copilot::product::success_body(data);

    assert(body["code"].asString() == "OK");
    assert(body["message"].asString() == "success");
    assert(body["data"]["status"].asString() == "ok");
}

void test_error_body_uses_json_null_data() {
    const Json::Value body =
        copilot::product::error_body(ErrorCode::route_not_found);

    assert(body["code"].asString() == "ROUTE_NOT_FOUND");
    assert(body["message"].asString() == "route not found");
    assert(body["data"].isNull());
}

void test_error_response_sets_http_status_and_content_type() {
    const auto response =
        copilot::product::error_response(ErrorCode::method_not_allowed);

    assert(response->statusCode() == drogon::k405MethodNotAllowed);
    assert(response->contentType() == drogon::CT_APPLICATION_JSON);
}

int main() {
    test_error_metadata();
    test_success_body();
    test_error_body_uses_json_null_data();
    test_error_response_sets_http_status_and_content_type();
}
```

- [ ] **Step 2: Run the test and verify it fails**

Run:

```powershell
E:\Software\MySY2\ucrt64\bin\cmake.exe --build build-product --target test_api_response
.\build-product\test_api_response.exe
```

Expected: program terminates with `success_body is not implemented`.

- [ ] **Step 3: Implement the response builders**

Replace `src/product/api_response.cpp` with:

```cpp
#include "copilot/product/api_response.hpp"

#include <utility>

namespace copilot::product {

Json::Value success_body(Json::Value data) {
    Json::Value body;
    body["code"] = to_string(ErrorCode::ok);
    body["message"] = error_message(ErrorCode::ok);
    body["data"] = std::move(data);
    return body;
}

Json::Value error_body(ErrorCode code) {
    Json::Value body;
    body["code"] = to_string(code);
    body["message"] = error_message(code);
    body["data"] = Json::Value(Json::nullValue);
    return body;
}

drogon::HttpResponsePtr json_response(
    drogon::HttpStatusCode status,
    const Json::Value& body) {
    auto response = drogon::HttpResponse::newHttpJsonResponse(body);
    response->setStatusCode(status);
    return response;
}

drogon::HttpResponsePtr success_response(Json::Value data) {
    return json_response(
        http_status(ErrorCode::ok),
        success_body(std::move(data)));
}

drogon::HttpResponsePtr error_response(ErrorCode code) {
    return json_response(http_status(code), error_body(code));
}

}  // namespace copilot::product
```

- [ ] **Step 4: Run the focused test**

Run:

```powershell
E:\Software\MySY2\ucrt64\bin\cmake.exe --build build-product --target test_api_response
.\build-product\test_api_response.exe
```

Expected: exit code `0`.

- [ ] **Step 5: Run all registered CMake tests**

Run:

```powershell
E:\Software\MySY2\ucrt64\bin\ctest.exe --test-dir build-product --output-on-failure
```

Expected: `core` and `api_response` pass.

- [ ] **Step 6: Commit when Git becomes available**

```powershell
git add tests/test_api_response.cpp src/product/api_response.cpp
git commit -m "feat: add unified JSON API responses"
```

### Task 4: Integrate Drogon into the CMake Build

**Files:**
- Modify: `CMakeLists.txt`
- Modify: `Makefile`

- [ ] **Step 1: Replace the top-level CMake file with explicit legacy and product dependencies**

Replace `CMakeLists.txt` with:

```cmake
cmake_minimum_required(VERSION 3.20)

project(cpp_ai_copilot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(BUILD_CTL OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_ORM OFF CACHE BOOL "" FORCE)
set(BUILD_BROTLI OFF CACHE BOOL "" FORCE)
set(BUILD_YAML_CONFIG OFF CACHE BOOL "" FORCE)
set(USE_SUBMODULE ON CACHE BOOL "" FORCE)

add_subdirectory(third_party/drogon)

add_library(copilot_core
    src/application.cpp
    src/config.cpp
    src/http.cpp
    src/logger.cpp
    src/router.cpp
)

target_include_directories(copilot_core PUBLIC include)

add_executable(cpp-ai-copilot-legacy
    src/main.cpp
    src/simple_server.cpp
)

target_link_libraries(cpp-ai-copilot-legacy PRIVATE copilot_core)

if(WIN32)
    target_link_libraries(cpp-ai-copilot-legacy PRIVATE ws2_32)
endif()

add_library(copilot_product_common
    src/product/api_response.cpp
    src/product/error_code.cpp
)

target_include_directories(copilot_product_common PUBLIC include)
target_link_libraries(copilot_product_common PUBLIC Drogon::Drogon)

if(
    EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/product/main.cpp"
    AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/product/health_controller.cpp"
)
    add_executable(cpp-ai-copilot-product
        src/product/main.cpp
        src/product/health_controller.cpp
    )

    target_include_directories(
        cpp-ai-copilot-product
        PRIVATE include src/product
    )
    target_link_libraries(
        cpp-ai-copilot-product
        PRIVATE copilot_product_common Drogon::Drogon
    )
endif()

add_executable(test_core tests/test_core.cpp)
target_link_libraries(test_core PRIVATE copilot_core)

add_executable(test_api_response tests/test_api_response.cpp)
target_link_libraries(test_api_response PRIVATE copilot_product_common)

enable_testing()
add_test(NAME core COMMAND test_core)
add_test(NAME api_response COMMAND test_api_response)
```

The product source files referenced here are created in Tasks 2, 3, and 5. During this task, configure first and build only targets whose source files already exist.

- [ ] **Step 2: Configure a clean Ninja build**

Run:

```powershell
E:\Software\MySY2\ucrt64\bin\cmake.exe -S . -B build-product -G Ninja -DCMAKE_BUILD_TYPE=Debug
```

Expected: configuration completes and reports Drogon v1.9.13. If CMake reports a missing package, install only that named UCRT64 package, record it, and rerun this exact command.

- [ ] **Step 3: Build and test the legacy target through CMake**

Run:

```powershell
E:\Software\MySY2\ucrt64\bin\cmake.exe --build build-product --target cpp-ai-copilot-legacy test_core
.\build-product\test_core.exe
```

Expected: both targets build and `test_core.exe` exits successfully.

- [ ] **Step 4: Add product convenience targets to Makefile**

Add these variables near the top of `Makefile`:

```make
CMAKE ?= cmake
PRODUCT_BUILD_DIR := build-product
```

Extend `.PHONY`:

```make
.PHONY: all test run clean product-configure product-build product-test product-run
```

Append:

```make
product-configure:
	$(CMAKE) -S . -B $(PRODUCT_BUILD_DIR) -G Ninja -DCMAKE_BUILD_TYPE=Debug

product-build: product-configure
	$(CMAKE) --build $(PRODUCT_BUILD_DIR) --target cpp-ai-copilot-product

product-test: product-configure
	$(CMAKE) --build $(PRODUCT_BUILD_DIR) --target test_core test_api_response
	ctest --test-dir $(PRODUCT_BUILD_DIR) --output-on-failure

product-run: product-build
	./$(PRODUCT_BUILD_DIR)/cpp-ai-copilot-product$(EXE) config/product.json
```

- [ ] **Step 5: Verify the original Makefile workflow remains intact**

Run:

```powershell
mingw32-make test
mingw32-make all
```

Expected: existing `build/test_core.exe` and `build/cpp-ai-copilot.exe` still build and run as before.

- [ ] **Step 6: Commit when Git becomes available**

```powershell
git add CMakeLists.txt Makefile
git commit -m "build: add Drogon product targets"
```

### Task 5: Add the Health Controller Test-First

**Files:**
- Create: `src/product/health_controller.hpp`
- Create: `src/product/health_controller.cpp`
- Modify: `tests/test_api_response.cpp`

- [ ] **Step 1: Add a pure health-data test**

Add to `include/copilot/product/api_response.hpp`:

```cpp
Json::Value health_data(
    const std::string& service_name,
    const std::string& version);
```

Add to `tests/test_api_response.cpp`:

```cpp
void test_health_data() {
    const Json::Value data =
        copilot::product::health_data("cpp-ai-copilot", "0.2.0");

    assert(data["status"].asString() == "ok");
    assert(data["service"].asString() == "cpp-ai-copilot");
    assert(data["version"].asString() == "0.2.0");
}
```

Call `test_health_data()` from `main()`.

- [ ] **Step 2: Run the test and verify it fails**

Run:

```powershell
E:\Software\MySY2\ucrt64\bin\cmake.exe --build build-product --target test_api_response
```

Expected: link failure for undefined `copilot::product::health_data`.

- [ ] **Step 3: Implement the health-data builder**

Add to `src/product/api_response.cpp`:

```cpp
Json::Value health_data(
    const std::string& service_name,
    const std::string& version) {
    Json::Value data;
    data["status"] = "ok";
    data["service"] = service_name;
    data["version"] = version;
    return data;
}
```

- [ ] **Step 4: Create the Drogon controller declaration**

Create `src/product/health_controller.hpp`:

```cpp
#pragma once

#include <drogon/HttpSimpleController.h>

namespace copilot::product {

class HealthController
    : public drogon::HttpSimpleController<HealthController> {
public:
    void asyncHandleHttpRequest(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback)
        override;

    PATH_LIST_BEGIN
    PATH_ADD("/health", drogon::Get);
    PATH_LIST_END
};

}  // namespace copilot::product
```

- [ ] **Step 5: Implement the health controller**

Create `src/product/health_controller.cpp`:

```cpp
#include "health_controller.hpp"

#include "copilot/product/api_response.hpp"

#include <drogon/drogon.h>
#include <string>

namespace copilot::product {

void HealthController::asyncHandleHttpRequest(
    const drogon::HttpRequestPtr&,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    const Json::Value& config = drogon::app().getCustomConfig();
    const std::string service_name =
        config.get("service_name", "cpp-ai-copilot").asString();
    const std::string version =
        config.get("version", "0.2.0").asString();

    callback(success_response(health_data(service_name, version)));
}

}  // namespace copilot::product
```

- [ ] **Step 6: Run the response tests**

Run:

```powershell
E:\Software\MySY2\ucrt64\bin\cmake.exe --build build-product --target test_api_response
.\build-product\test_api_response.exe
```

Expected: exit code `0`.

- [ ] **Step 7: Commit when Git becomes available**

```powershell
git add include/copilot/product/api_response.hpp src/product tests/test_api_response.cpp
git commit -m "feat: add product health controller"
```

### Task 6: Configure Product Startup and Global Errors

**Files:**
- Create: `config/product.json`
- Create: `src/product/main.cpp`

- [ ] **Step 1: Create the product configuration**

Create `config/product.json`:

```json
{
  "listeners": [
    {
      "address": "127.0.0.1",
      "port": 18081,
      "https": false
    }
  ],
  "app": {
    "number_of_threads": 1,
    "enable_session": false,
    "document_root": "./public",
    "upload_path": "uploads",
    "file_types": [],
    "log": {
      "log_path": "",
      "logfile_base_name": "",
      "log_size_limit": 100000000,
      "log_level": "INFO"
    }
  },
  "custom_config": {
    "service_name": "cpp-ai-copilot",
    "version": "0.2.0"
  }
}
```

- [ ] **Step 2: Create the product entry point**

Create `src/product/main.cpp`:

```cpp
#include "copilot/product/api_response.hpp"
#include "copilot/product/error_code.hpp"
#include "health_controller.hpp"

#include <drogon/drogon.h>

#include <cstdint>
#include <exception>
#include <string>

namespace {

using copilot::product::ErrorCode;

ErrorCode error_code_for_status(drogon::HttpStatusCode status) {
    switch (status) {
        case drogon::k400BadRequest:
            return ErrorCode::invalid_request;
        case drogon::k404NotFound:
            return ErrorCode::route_not_found;
        case drogon::k405MethodNotAllowed:
            return ErrorCode::method_not_allowed;
        default:
            return ErrorCode::internal_error;
    }
}

}  // namespace

int main(int argc, char* argv[]) {
    const std::string config_path =
        argc > 1 ? argv[1] : "config/product.json";

    auto& app = drogon::app();
    app.loadConfigFile(config_path);

    app.setDefaultHandler(
        [](const drogon::HttpRequestPtr&,
           std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            callback(copilot::product::error_response(
                ErrorCode::route_not_found));
        });

    app.setCustomErrorHandler(
        [](drogon::HttpStatusCode status,
           const drogon::HttpRequestPtr&) {
            return copilot::product::error_response(
                error_code_for_status(status));
        });

    app.setExceptionHandler(
        [](const std::exception& exception,
           const drogon::HttpRequestPtr&,
           std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
            LOG_ERROR << "Unhandled request exception: " << exception.what();
            callback(copilot::product::error_response(
                ErrorCode::internal_error));
        });

    app.registerPreSendingAdvice(
        [](const drogon::HttpRequestPtr& request,
           const drogon::HttpResponsePtr& response) {
            const std::int64_t elapsed_microseconds =
                trantor::Date::now().microSecondsSinceEpoch() -
                request->creationDate().microSecondsSinceEpoch();
            LOG_INFO << request->methodString() << ' ' << request->path()
                     << " -> " << response->statusCode()
                     << " in " << elapsed_microseconds / 1000.0 << "ms";
        });

    app.run();
}
```

- [ ] **Step 3: Build the complete product target**

Run:

```powershell
E:\Software\MySY2\ucrt64\bin\cmake.exe -S . -B build-product -G Ninja -DCMAKE_BUILD_TYPE=Debug
E:\Software\MySY2\ucrt64\bin\cmake.exe --build build-product --target cpp-ai-copilot-product
```

Expected: `cpp-ai-copilot-product.exe` builds successfully.

- [ ] **Step 4: Run all unit tests**

Run:

```powershell
E:\Software\MySY2\ucrt64\bin\ctest.exe --test-dir build-product --output-on-failure
mingw32-make test
```

Expected: all CMake tests pass and the teaching test still passes.

- [ ] **Step 5: Commit when Git becomes available**

```powershell
git add config/product.json src/product/main.cpp
git commit -m "feat: add Drogon product startup and global errors"
```

### Task 7: Run HTTP Integration Verification

**Files:**
- Verify: `config/product.json`
- Verify: `src/product/main.cpp`
- Verify: `src/product/health_controller.cpp`

- [ ] **Step 1: Start the product server in the background**

Run:

```powershell
$server = Start-Process -FilePath '.\build-product\cpp-ai-copilot-product.exe' -ArgumentList 'config/product.json' -WindowStyle Hidden -PassThru
Start-Sleep -Seconds 2
```

Expected: `$server.HasExited` is `False`.

- [ ] **Step 2: Verify the health endpoint**

Run:

```powershell
$health = Invoke-RestMethod -Uri 'http://127.0.0.1:18081/health' -Method Get
$health | ConvertTo-Json -Depth 5
```

Expected:

```json
{
  "code": "OK",
  "message": "success",
  "data": {
    "status": "ok",
    "service": "cpp-ai-copilot",
    "version": "0.2.0"
  }
}
```

- [ ] **Step 3: Verify the unified 404**

Run:

```powershell
try {
    Invoke-RestMethod -Uri 'http://127.0.0.1:18081/missing' -Method Get
} catch {
    $_.Exception.Response.StatusCode.value__
    $_.ErrorDetails.Message
}
```

Expected: status `404`; body contains `"code":"ROUTE_NOT_FOUND"` and `"data":null`.

- [ ] **Step 4: Verify the unified 405**

Run:

```powershell
try {
    Invoke-RestMethod -Uri 'http://127.0.0.1:18081/health' -Method Post
} catch {
    $_.Exception.Response.StatusCode.value__
    $_.ErrorDetails.Message
}
```

Expected: status `405`; body contains `"code":"METHOD_NOT_ALLOWED"` and `"data":null`.

- [ ] **Step 5: Verify access logs**

Inspect the captured server console output or run the server interactively once.

Expected lines include:

```text
GET /health -> 200 in
GET /missing -> 404 in
POST /health -> 405 in
```

- [ ] **Step 6: Stop only the server process created in Step 1**

Run:

```powershell
Stop-Process -Id $server.Id
```

Expected: the product server exits and port `18081` is released.

- [ ] **Step 7: Run the complete verification suite again**

Run:

```powershell
mingw32-make test
E:\Software\MySY2\ucrt64\bin\ctest.exe --test-dir build-product --output-on-failure
```

Expected: every test passes.

### Task 8: Document the Product Skeleton

**Files:**
- Modify: `README.md`
- Modify: `../../03_实现路线/01第一周c++Web框架/03_从教学HTTP服务器升级成熟Web框架过程记录.md`

- [ ] **Step 1: Update README with the two-server model**

Add a section explaining:

```markdown
## 两个后端版本

- `cpp-ai-copilot-legacy`：手写 Winsock 教学版，用于理解 HTTP 请求链路。
- `cpp-ai-copilot-product`：Drogon 产品版，后续 MySQL、Redis、上传、SSE 和 RAG 功能都在此版本开发。

## 构建产品版

```powershell
mingw32-make product-build
mingw32-make product-test
mingw32-make product-run
```

产品版默认监听 `http://127.0.0.1:18081`。
```

- [ ] **Step 2: Append exact dependency and build records**

Append to the process record:

```markdown
## Drogon 产品骨架实际落地记录

- Drogon 版本：v1.9.13
- Drogon commit：使用 `git -C third_party/drogon rev-parse HEAD` 的实际输出
- Trantor commit：使用 `git -C third_party/drogon submodule status` 的实际输出
- CMake：4.2.3
- Ninja：1.13.2
- 构建方式：顶层 CMake `add_subdirectory(third_party/drogon)`
- 首次关闭：`BUILD_CTL`、`BUILD_EXAMPLES`、`BUILD_ORM`、`BUILD_BROTLI`、`BUILD_YAML_CONFIG`
- 保留 `USE_SUBMODULE=ON`，因为 Drogon 需要 Trantor 网络库

产品版新增能力：

1. Drogon 异步 HTTP 服务。
2. `GET /health`。
3. 统一 `code/message/data` JSON。
4. 统一 404、405 和 500。
5. 请求方法、路径、状态码和耗时日志。
6. 独立的 `config/product.json`。

验证结果：

- `GET /health`：200 + `OK`
- `GET /missing`：404 + `ROUTE_NOT_FOUND`
- `POST /health`：405 + `METHOD_NOT_ALLOWED`
- 教学版 `mingw32-make test`：通过
- 产品版 `ctest --test-dir build-product --output-on-failure`：通过
```

Insert the two command outputs directly instead of retaining the explanatory phrases.

- [ ] **Step 3: Run final documentation and placeholder checks**

Run:

```powershell
Select-String -Path README.md,'..\..\03_实现路线\01第一周c++Web框架\03_从教学HTTP服务器升级成熟Web框架过程记录.md' -Pattern 'TBD|TODO|实际输出|待补充' -Encoding UTF8
```

Expected: no matches.

- [ ] **Step 4: Run final build and tests**

Run:

```powershell
mingw32-make test
E:\Software\MySY2\ucrt64\bin\cmake.exe --build build-product --target cpp-ai-copilot-product test_core test_api_response
E:\Software\MySY2\ucrt64\bin\ctest.exe --test-dir build-product --output-on-failure
```

Expected: all commands succeed.

- [ ] **Step 5: Commit when Git becomes available**

```powershell
git add README.md ../../03_实现路线/01第一周c++Web框架/03_从教学HTTP服务器升级成熟Web框架过程记录.md
git commit -m "docs: document Drogon product skeleton"
```

## Plan Self-Review

- Spec coverage: dependency pinning, independent legacy/product targets, health endpoint, unified success and error bodies, 404/405/500 handling, configuration, access logs, unit tests, integration checks, and process documentation are all covered.
- Scope: MySQL, Redis, Docker Compose, SSE, model APIs, RAG, uploads, and authentication remain outside this plan.
- Type consistency: all tasks use `ErrorCode`, `success_body`, `error_body`, `success_response`, `error_response`, and `health_data` consistently.
- Dependency basis: Drogon v1.9.13 is pinned because it is the latest official release as of June 15, 2026; the plan uses APIs present in its official `HttpAppFramework.h`.

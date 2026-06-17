#include <filesystem>
#include <iostream>

int main() {
    std::filesystem::path p = ".";
    std::cout << std::filesystem::exists(p) << std::endl;
    return 0;
}

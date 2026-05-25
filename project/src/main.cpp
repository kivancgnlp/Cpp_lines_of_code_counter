


#include <filesystem>
#include <iostream>
#include <fmt/core.h>
#include <spdlog/spdlog.h>

void list_files_in_the_directory(const std::filesystem::path& path) {
        spdlog::info("Listing the files in the directory {}", path.string());

        for (const auto& entry : std::filesystem::directory_iterator(path)) {
                spdlog::info("{}", entry.path().string());
        }
}

int main(int argc, const char * argv[]) {



        spdlog::info("App start");


        list_files_in_the_directory(".");

        spdlog::info("App end");

}

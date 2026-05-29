//
// Created by Kivanc Gunalp on 29.05.2026.
//

#include "Utils.h"



std::optional<std::uintmax_t> get_file_size(const std::filesystem::path &path) {

    std::error_code ec2{};
    const std::uintmax_t size = std::filesystem::file_size(path, ec2);
    if (ec2) {
        spdlog::warn("Unable to get file size for {}, ec : {}", path.string(), ec2.message());
        return std::nullopt;
    }else {
        return size;
    }
}

std::string get_human_readable_size(std::uintmax_t size) {
    double size_float = size;;
    unsigned suffix = 0;

    while (size_float > 1024) {
        size_float /= 1024;
        suffix++;
    }

    constexpr std::array<const char *, 4> prefix_str = {"bytes", "KB", "MB", "GB"};

    return fmt::format("{:.2f} {}",size_float,prefix_str.at(suffix));
}

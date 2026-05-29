//
// Created by Kivanc Gunalp on 29.05.2026.
//

#ifndef DISK_USAGE_ANALYZER_UTILS_H
#define DISK_USAGE_ANALYZER_UTILS_H

#include <filesystem>
#include <spdlog/spdlog.h>


std::optional<std::uintmax_t> get_file_size(const std::filesystem::path &path);
std::string get_human_readable_size(std::uintmax_t size);

#endif //DISK_USAGE_ANALYZER_UTILS_H
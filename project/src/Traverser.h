//
// Created by Kivanc Gunalp on 30.05.2026.
//

#ifndef DISK_USAGE_ANALYZER_TRAVERSER_H
#define DISK_USAGE_ANALYZER_TRAVERSER_H

#include <filesystem>
#include "Stats.h"

void list_files_in_the_directory(const std::filesystem::path& path_base, Stats &stats,Stats &global_stats, unsigned current_depth);

#endif //DISK_USAGE_ANALYZER_TRAVERSER_H
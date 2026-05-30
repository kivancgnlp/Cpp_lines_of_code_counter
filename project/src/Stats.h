//
// Created by Kivanc Gunalp on 29.05.2026.
//

#ifndef DISK_USAGE_ANALYZER_STATS_H
#define DISK_USAGE_ANALYZER_STATS_H

#include <filesystem>

#include <iostream>
#include <ostream>
#include <map>

#include <spdlog/spdlog.h>

#include "Utils.h"



struct Extension_Stat {
        unsigned counter;
        std::uintmax_t acc_size;

        std::map<std::string, Extension_Stat>::mapped_type &operator+=(const Extension_Stat & second) {
                counter += second.counter;
                acc_size += second.acc_size;
                return *this;
        }
};

class Stats {
        unsigned processed_files{};
        unsigned processed_folders{};
        unsigned unclassified_entries{};
        std::uintmax_t total_size{};
        unsigned file_get_size_error_count{};
        unsigned directory_info_get_error_count{};

        std::map<std::string, Extension_Stat> extension_stats;
        std::vector<std::pair<std::string, std::uintmax_t>> biggest_files;


        public:
        template <bool VERBOSE>
        std::string to_string() const;

        //Stats &operator+=(const Stats & stats);
        void migrate_stats(Stats &&from_stat);

        void update_biggest_files(std::string &&file_path,std::uintmax_t size) {

                constexpr unsigned TOP_LIST_SIZE = 10;

                if (biggest_files.size() == TOP_LIST_SIZE) {

                        if (biggest_files.back().second > size) {
                                // if it is smaller than the smallest do nothing
                                return;
                        }
                }
                biggest_files.emplace_back(std::move(file_path), size);

                std::ranges::sort(biggest_files, [](const auto & a, const auto & b) {
                        return a.second > b.second;
                });


                while (biggest_files.size() > TOP_LIST_SIZE){
                        biggest_files.pop_back();
                }

        }


        void add_file_size_stat(const std::filesystem::path &path, std::uintmax_t size) {
                total_size += size;
                processed_files++;
                update_biggest_files(path.string(), size);
        }


        void update_extension_stats(const std::string& extension, std::uintmax_t file_size);
        void increment_file_get_size_error_count();
        void increment_directory_info_error_count();
        void increment_processed_folder_count();
        void increment_unclassified_entries_count();
};


#endif //DISK_USAGE_ANALYZER_STATS_H

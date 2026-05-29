//
// Created by Kivanc Gunalp on 29.05.2026.
//

#include "Stats.h"

Stats & Stats::operator+=(const Stats &stats) {
    processed_files += stats.processed_files;
    processed_folders += stats.processed_folders;
    unclassified_entries += stats.unclassified_entries;
    total_size += stats.total_size;

    for (const auto & entry : stats.extension_stats) {
        extension_stats[entry.first] += entry.second;
    }

    for (const auto & entry : stats.biggest_files) {
       add_file_size_stat(entry.first, entry.second);
    }
    return *this;
}

void Stats::update_extension_stats(const std::string &extension, std::uintmax_t file_size) {

    if ( auto ex = extension_stats.find(extension); ex != extension_stats.end()){
        ex->second.counter++;
        ex->second.acc_size += file_size;
    }else {
        extension_stats.insert({extension, {1,file_size}});
    }

}

void Stats::increment_file_get_size_error_count() {
    file_get_size_error_count++;
}

void Stats::increment_directory_info_error_count() {
    directory_info_get_error_count++;
}

void Stats::increment_processed_folder_count() {
    processed_folders++;
}

void Stats::increment_unclassified_entries_count() {
    unclassified_entries++;
}


template<bool VERBOSE>
std::string Stats::to_string() const {
    std::stringstream ss;

    if constexpr (VERBOSE) {
        ss << "\n processed_files: " << processed_files
                        << "\n processed_folders: " << processed_folders
                        << "\n unclassified_entries: " << unclassified_entries
                        << "\n size: " << get_human_readable_size(total_size);

        std::vector<std::pair<std::string,Extension_Stat>> sorted_ext_stats;

        sorted_ext_stats.reserve(extension_stats.size());

        for (const auto &entry : extension_stats) {
            sorted_ext_stats.emplace_back(entry.first, entry.second);
        }

        std::sort(sorted_ext_stats.begin(), sorted_ext_stats.end(),[](const std::pair<std::string,Extension_Stat>& p1, const std::pair<std::string,Extension_Stat>& p2) {
                return p1.second.acc_size > p2.second.acc_size;
        });

        ss << "\n File extension stats : \n",
                        std::ranges::for_each(sorted_ext_stats, [&ss](const std::pair<std::string,Extension_Stat>& p) {
                                ss  << std::setw(12) <<p.first << "\t acc size : " << std::setw(12) << get_human_readable_size(p.second.acc_size) << "\t count : "<< p.second.counter << '\n';
                        });

        ss << "\n Biggest files : \n",
                   std::ranges::for_each(biggest_files, [&ss](const std::pair<std::string,std::uintmax_t>& p) {
                           ss << std::setw(12) << get_human_readable_size(p.second) << '\t' << p.first <<'\n';
                   });


    }else {
        ss << "size : " << get_human_readable_size(total_size);

    }

    if (directory_info_get_error_count) {
        ss << "\n directory_info_get_error_count: " << directory_info_get_error_count;
    }

    if (file_get_size_error_count) {
        ss << "\n file_get_size_error_count: " << file_get_size_error_count;
    }


    return ss.str();
}


template std::string Stats::to_string<true>() const;
template std::string Stats::to_string<false>() const;
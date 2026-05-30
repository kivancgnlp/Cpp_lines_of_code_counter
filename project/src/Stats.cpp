//
// Created by Kivanc Gunalp on 29.05.2026.
//

#include "Stats.h"



void Stats::migrate_stats(Stats &&from_stat) {
    processed_files += from_stat.processed_files;from_stat.processed_files = 0;
    processed_folders += from_stat.processed_folders;from_stat.processed_folders = 0;
    unclassified_entries += from_stat.unclassified_entries;from_stat.unclassified_entries = 0;

    total_size += from_stat.total_size; from_stat.total_size = 0;
    file_get_size_error_count += from_stat.file_get_size_error_count; from_stat.file_get_size_error_count = 0;
    directory_info_get_error_count += from_stat.directory_info_get_error_count; from_stat.directory_info_get_error_count = 0;


    for (auto& [ext, stat] : from_stat.extension_stats) {
        extension_stats[ext] += stat;     // operator[] default-constructs, then += accumulates
    }

    from_stat.extension_stats.clear();

    for (auto & entry : from_stat.biggest_files) {
        update_biggest_files(std::move(entry.first),entry.second);
    }

    from_stat.biggest_files.clear();
}

void Stats::update_biggest_files(std::string &&file_path, std::uintmax_t size) {

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

void Stats::add_file_size_stat(const std::filesystem::path &path, std::uintmax_t size) {
    total_size += size;
    processed_files++;
    update_biggest_files(path.string(), size);
}

void Stats::update_extension_stats(const std::string &extension, std::uintmax_t file_size) {

    if (const auto ex = extension_stats.find(extension); ex != extension_stats.end()){
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
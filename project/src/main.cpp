


#include <filesystem>

#include <iostream>
#include <ostream>
#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <CLI/CLI.hpp>
#include <cpptrace/from_current.hpp>

static std::optional<std::uintmax_t> get_file_size(const std::filesystem::path &path);
std::string get_human_readable_size(std::uintmax_t size);

struct Extension_Stat {
        unsigned counter;
        std::uintmax_t acc_size;

        std::map<std::string, Extension_Stat>::mapped_type &operator+=(const Extension_Stat & second) {
                counter += second.counter;
                acc_size += second.acc_size;
                return *this;
        }
};

struct Stats {
        unsigned processed_files = 0;
        unsigned processed_folders = 0;
        unsigned unclassified_entries = 0;
        std::uintmax_t total_size = 0;
        unsigned file_get_size_error_count = 0;
        unsigned directory_info_get_error_count = 0;

        std::map<std::string, Extension_Stat> extension_stats;



        template <bool VERBOSE>
        std::string to_string() const {
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

        Stats &operator+=(const Stats & stats) {
                processed_files += stats.processed_files;
                processed_folders += stats.processed_folders;
                unclassified_entries += stats.unclassified_entries;
                total_size += stats.total_size;

                for (const auto & entry : stats.extension_stats) {
                        extension_stats[entry.first] += entry.second;
                }
                return *this;
        }

        void update_extension_stats(const std::string& extension, std::uintmax_t file_size) {

                if ( auto ex = extension_stats.find(extension); ex != extension_stats.end()){
                        ex->second.counter++;
                        ex->second.acc_size += file_size;
                }else {
                        extension_stats.insert({extension, {1,file_size}});
                }

        }
};

void list_files_in_the_directory(const std::filesystem::path& path, Stats &stats,Stats &global_stats, unsigned current_depth) {
        spdlog::debug("Listing the files in the directory {}", path.string());

        std::error_code ec{};
        std::filesystem::directory_iterator dir_it = std::filesystem::directory_iterator(path,ec);

        if (ec) {
                spdlog::warn("Error in listing the files in the directory {}, {}", path.string(),ec.message());
                stats.directory_info_get_error_count++;
                return;
        }

        const std::filesystem::directory_iterator end{};

        while (dir_it != end) {
                const std::filesystem::path &path = dir_it->path();

                if (is_regular_file(path)) {
                        spdlog::log(spdlog::level::debug,"Processing file : {}", path.string());

                        if (std::optional<std::uintmax_t> size = get_file_size(path)) {
                                stats.total_size += size.value();
                                stats.processed_files++;
                                if (path.has_extension()) {
                                        stats.update_extension_stats(path.extension().string(), size.value());
                                }
                        }else {
                                stats.file_get_size_error_count++;
                        }

                }else if (is_directory(path)) {
                        spdlog::debug("Processing directory : {}", path.string());
                        list_files_in_the_directory(path, stats,global_stats, current_depth + 1);
                        stats.processed_folders++;
                }else {
                        spdlog::warn("Unknown entry : {}", path.string());
                        stats.unclassified_entries++;
                }


                {
                        std::error_code ec{};
                        dir_it.increment(ec);

                        if (ec) {
                                spdlog::warn("Could not process file : {}", path.string());
                                break;
                        }

                }

        }

        if (current_depth == 1) {
                spdlog::info("Processing completed for folder : {}, folder stats : {}", path.string(), stats.to_string<false>());
                global_stats += stats;
                stats = Stats{};
        }


}

int main(int argc, const char * argv[]) {

        CLI::App app{"Kiv disk usage analyzer"};

        std::string path_str = ".";
        CLI::Option *opt = app.add_option("-p,--path,path", path_str, "Path to files");
        CLI11_PARSE(app, argc, argv);
        spdlog::info("Calculating disk usage starting with base folder {}", path_str);

        Stats stats{};
        Stats global_stats{};

        CPPTRACE_TRY {
                list_files_in_the_directory(path_str, stats,global_stats,0);
        } CPPTRACE_CATCH(const std::exception& e) {
                auto trace = cpptrace::from_current_exception().to_string();
                spdlog::error("Exception: {}\n{}", e.what(), trace);
        }


        spdlog::info("Total stats : {}", global_stats.to_string<true>());

}


static std::optional<std::uintmax_t> get_file_size(const std::filesystem::path &path) {

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

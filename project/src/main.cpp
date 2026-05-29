


#include <filesystem>

#include <iostream>
#include <ostream>
#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <CLI/CLI.hpp>
#include <cpptrace/from_current.hpp>

#include "Utils.h"
#include "Stats.h"


void list_files_in_the_directory(const std::filesystem::path& path, Stats &stats,Stats &global_stats, unsigned current_depth) {
        spdlog::debug("Listing the files in the directory {}", path.string());

        std::error_code ec{};
        std::filesystem::directory_iterator dir_it = std::filesystem::directory_iterator(path,ec);

        if (ec) {
                spdlog::warn("Error in listing the files in the directory {}, {}", path.string(),ec.message());
                stats.increment_directory_info_error_count();
                return;
        }

        const std::filesystem::directory_iterator end{};

        while (dir_it != end) {
                const std::filesystem::path &path = dir_it->path();

                if (is_regular_file(path)) {
                        spdlog::log(spdlog::level::debug,"Processing file : {}", path.string());

                        if (std::optional<std::uintmax_t> size = get_file_size(path)) {
                                stats.add_file_size_stat(path,size.value());

                                if (path.has_extension()) {
                                        stats.update_extension_stats(path.extension().string(), size.value());
                                }
                        }else {
                                stats.increment_file_get_size_error_count();

                        }

                }else if (is_directory(path)) {
                        spdlog::debug("Processing directory : {}", path.string());
                        list_files_in_the_directory(path, stats,global_stats, current_depth + 1);
                        stats.increment_processed_folder_count();

                }else {
                        spdlog::warn("Unknown entry : {}", path.string());
                        stats.increment_unclassified_entries_count();

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

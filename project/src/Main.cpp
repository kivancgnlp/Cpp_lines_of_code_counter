


#include <filesystem>

#include <iostream>
#include <ostream>

#include <spdlog/spdlog.h>

#include <CLI/CLI.hpp>
#include <cpptrace/from_current.hpp>

#include "Stats.h"
#include "Traverser.h"


int main(int argc, const char * argv[]) {

        CLI::App app{"Kiv disk usage analyzer"};

        std::string path_str = ".";
        app.add_option("-p,--path,path", path_str, "Path to files");
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

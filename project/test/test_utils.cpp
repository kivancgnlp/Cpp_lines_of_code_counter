//
// Created by Kivanc Gunalp on 30.05.2026.
//

#include <gtest/gtest.h>
#include "../src/Utils.h"
#include "../src/Stats.h"

TEST(HumanReadable, PowerOfKiloBoundary) {
    EXPECT_EQ(get_human_readable_size(1023), "1023.00 bytes");
    EXPECT_EQ(get_human_readable_size(1024), "1.00 KB");      // the boundary bug
    EXPECT_EQ(get_human_readable_size(1048576), "1.00 MB");
}
TEST(HumanReadable, CapsAtLargestUnitNoThrow) {
    EXPECT_NO_THROW(get_human_readable_size(1ULL << 50));     // the guard bug
}

TEST(Stats, StatsMigrateTest) {
    Stats donor_stat{};
    donor_stat.increment_processed_folder_count();

    Stats receiving_stat;
    receiving_stat.increment_processed_folder_count();

    receiving_stat.migrate_stats(std::move(donor_stat));

    EXPECT_EQ(receiving_stat.get_processed_folder_count(),2);

}
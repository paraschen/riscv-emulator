#include <gtest/gtest.h>
#include "cpu.h"
#include "memory.h"
#include "bus.h"

TEST(CPUTest, InitialState) {
    auto bus = std::make_shared<Bus>();
    auto memory = std::make_shared<Memory>(1024);
    bus->add_device(0x80000000, memory);
    
    CPU cpu(bus);
    
    EXPECT_EQ(cpu.get_pc(), 0x80000000);
    // Add more tests
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

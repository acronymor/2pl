CMAKE_MINIMUM_REQUIRED(VERSION 3.22)
SET(CMAKE_CXX_STANDARD 20)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

LIST(APPEND test_libraries gtest gtest_main)

ADD_EXECUTABLE(coarse comm/config.h coarse/coarse_test.cc)
TARGET_LINK_LIBRARIES(coarse ${test_libraries})
ADD_TEST(NAME coarse COMMAND coarse)

ADD_EXECUTABLE(fine comm/config.h fine/pl_test.cc fine/lock_manager.cc fine/transaction_manager.cc)
TARGET_LINK_LIBRARIES(fine ${test_libraries})
ADD_TEST(NAME fine COMMAND fine)

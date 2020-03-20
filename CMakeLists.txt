cmake_minimum_required(VERSION 2.6.4)
project(CFS_CLYDE_EPS C)

include(../ComponentSettings.cmake)

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)
include_directories(${hwlib_MISSION_DIR}/fsw/public_inc)

aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(clyde_eps ${APP_SRC_FILES})

# unit tests
aux_source_directory(fsw/unit_test UT_SRC_FILES)
#add_mission_unit_test(test_eps ${UT_SRC_FILES} ${APP_SRC_FILES} LINK_HWLIB)


cmake_minimum_required(VERSION 3.28.3) 

# Definition of project main propreties.
# MUST be adapted to each project.
project(Argus LANGUAGES C)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)
set(EXEC_NAME "argus")
set(LIBRARIES m readline GL GLEW SDL2 SDL2_image SDL2_ttf )
set(DEBUG_FLAGS -g -Wall -Wextra)
set(RELEASE_FLAGS -O3)

# Get all sources files of the project.
file(GLOB_RECURSE TESTS "test/*.c")
file(GLOB_RECURSE SOURCES "src/*.c" "src/*.h")

# Exclude the file containing the main function for testing.
set(SOURCES_TESTS "")
foreach(SOURCE ${SOURCES})
    file(READ ${SOURCE} FILE_CONTENTS)
    if(NOT FILE_CONTENTS MATCHES "int[ \t\r\n]+main[ \t\r\n]*\\(")
        list(APPEND SOURCES_TESTS ${SOURCE})
    endif()
endforeach()



# Function to add an executable for tests.
function(add_test_executable TEST_NAME ...)
    set(TEST_SOURCES ${ARGV})
    list(REMOVE_AT TEST_SOURCES 0)
    add_executable(${TEST_NAME} test/${TEST_NAME}.c ${TEST_SOURCES})
    target_compile_options(${TEST_NAME} PRIVATE ${DEBUG_FLAGS})
    target_link_libraries(${TEST_NAME} ${LIBRARIES})
    add_dependencies(build_tests ${TEST_NAME})
    add_custom_command(
        TARGET run_tests
        COMMAND ${CMAKE_COMMAND} -E echo "Running ${TEST_NAME}..."
        COMMAND $<TARGET_FILE:${TEST_NAME}>
        DEPENDS ${TEST_NAME}
    )
endfunction()



# Add the main executable, with Release and Debug mode.
if (SOURCES) 
    add_executable(${EXEC_NAME} ${SOURCES} ${HEADERS} ${TEMPLATES})
    target_link_libraries(${EXEC_NAME} ${LIBRARIES})
    target_compile_options(${EXEC_NAME} PRIVATE
        $<$<CONFIG:Debug>:${DEBUG_FLAGS}> 
        $<$<CONFIG:Release>:${RELEASE_FLAGS}>               
    )
endif()

# Add tests only for Debug mode.
if(CMAKE_BUILD_TYPE STREQUAL "Debug")

    # Add custom targets to build and run all tests.
    add_custom_target(
        build_tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    add_custom_target(
        run_tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    # Add each testing executable.
    foreach(TEST_FILE ${TESTS})
        get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
        add_test_executable(${TEST_NAME} ${SOURCES_TESTS})
    endforeach()

endif()

# Add a rule to build the root Makefile.
add_custom_target(
    build_root_makefile
    COMMAND ./build_root_makefile.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Add a rule to run the main executable.
add_custom_target(
    run 
    COMMAND ./${EXEC_NAME}
    DEPENDS ${EXEC_NAME}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

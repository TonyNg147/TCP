include(FetchContent)


FetchContent_Declare(
  doctest
  GIT_REPOSITORY https://github.com/doctest/doctest.git
  GIT_TAG v2.4.11
)

FetchContent_MakeAvailable(doctest)

add_executable(test_event_driven test_suite.cpp)
target_link_libraries(test_event_driven PRIVATE EventDriven)
target_link_libraries(test_event_driven PRIVATE doctest)

include(${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake)
doctest_discover_tests(test_event_driven)


if(TEST)
    add_custom_command(
        TARGET test_event_driven 
        POST_BUILD
        COMMAND test_event_driven
        COMMENT "Running test since TEST=ON"
    )
endif()
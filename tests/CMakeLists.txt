find_package(GTest REQUIRED)

add_executable(paranoid_tests
    smoke_test.cpp
    vec2_test.cpp
    rect_test.cpp
    animation_test.cpp
    animated_sprite_test.cpp
    sprite_test.cpp
    texture_test.cpp
    sound_test.cpp
    music_test.cpp
    stage_map_test.cpp
    resource_holder_test.cpp
    game_smoke_test.cpp
    play_state_test.cpp
)

target_compile_definitions(paranoid_tests
    PRIVATE
        TEST_MEDIA_DIR="${CMAKE_SOURCE_DIR}/media"
)

target_link_libraries(paranoid_tests
    PRIVATE
        GTest::gtest_main
        paranoid_game
        paranoid_options
)

include(GoogleTest)
gtest_discover_tests(paranoid_tests)

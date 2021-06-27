# CMakeList.txt: проект CMake для hw7; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

if(DEFINED ENV{GITHUB_RUN_NUMBER})
    project ("bayan" VERSION 0.0.$ENV{GITHUB_RUN_NUMBER})
else ()
    project ("bayan")
endif()

find_package(Boost COMPONENTS program_options filesystem regex REQUIRED)

include_directories(${Boost_INCLUDE_DIR})

# Добавьте источник в исполняемый файл этого проекта.
add_executable (bayan "main.cpp" "duplicate_finder.cpp" "duolicate_finder.hpp")

target_link_libraries(bayan
        ${Boost_LIBRARIES}
        ) 

if (MSVC)
    target_compile_options(bayan PUBLIC
        /W4
    )
else ()
    target_compile_options(bayan PUBLIC
        -Wall -Wextra -pedantic -Werror
    )
endif()

set_target_properties(bayan PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

set_target_properties(bayan PROPERTIES
    COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
)

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
install(TARGETS bayan RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)
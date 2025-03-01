# CMakeLists.txt
# Cubos project root build configuration

cmake_minimum_required(VERSION 3.20.0)
project(cubos VERSION 0.1.0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

set(CMAKE_CXX_STANDARD 20)

# Should clang-tidy errors be automatically fixed?
option(FIX_CLANG_TIDY_ERRORS "Automatically fix clang-tidy errors" OFF)

# Enable coverage reports
option(ENABLE_COVERAGE "Generate coverage report" OFF)
if(ENABLE_COVERAGE)
    include(CodeCoverage)
    append_coverage_compiler_flags()

    setup_target_for_coverage_lcov(
        NAME coverage
        EXECUTABLE core/tests/cubos-core-tests
        DEPENDENCIES cubos-core-tests

        EXCLUDE "core/lib/*" "core/tests/*" "tools/quadrados" "*-components.cpp"

        LCOV_ARGS "--no-external"
    )
endif()

# Enable CCache
find_program(CCACHE_EXE NAMES "ccache")
if(CCACHE_EXE)
    option(USE_CCACHE "Enable CCache" ON)
    if(USE_CCACHE)
        if (CMAKE_HOST_WIN32)
            option(CCACHE_VERSION "Pinned CCache Version")

            # find_program only finds Chocolatey's shim which is unable to be used with another name, the original executable must be used.
            set(CCACHE_PATH C:/ProgramData/chocolatey/lib/ccache/tools/ccache-${CCACHE_VERSION}-windows-x86_64/ccache.exe)

            file(COPY_FILE
                ${CCACHE_PATH} ${CMAKE_BINARY_DIR}/cl.exe
                ONLY_IF_DIFFERENT)

            set(CMAKE_VS_GLOBALS
                "CLToolExe=cl.exe"
                "CLToolPath=${CMAKE_BINARY_DIR}"
                "TrackFileAccess=false"
                "UseMultiToolTask=true"
                "DebugInformationFormat=OldStyle"
            )
        else ()
            set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_EXE}")
        endif()
    endif()
endif()

# Function which configures a target with the options common to all CUBOS. targets.
function(cubos_common_target_options target)
    # Enable all warnings and treat them as errors
    target_compile_options(${target} PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:
            /Zc:preprocessor # Enable preprocessor conformance mode - required for __VA_ARGS__ to work correctly
            /W4
            /WX
            /wd4996  # Disable warning about deprecated functions
            /wd4458  # Disable warning about shadowing member variables
            /wd4456  # Disable warning about shadowing local variables
            /wd4335  # Disable warning about Mac file format
            /wd5030> # Introduces false positives for [[cubos::component]]
            
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:
            -Wall
            -Wextra
            -pedantic
            -Wconversion
            -Werror
            -Wno-attributes
            -Wno-unknown-attributes> # Introduces false positives for [[cubos::component]]
    )

    # Enable clang-tidy
    find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
    if(CLANG_TIDY_EXE)
        option(USE_CLANG_TIDY "Enable clang-tidy" OFF)
        if(USE_CLANG_TIDY)
            set(CLANG_TIDY_COMMAND "${CLANG_TIDY_EXE}")
            if(FIX_CLANG_TIDY_ERRORS)
                set(CLANG_TIDY_COMMAND "${CLANG_TIDY_COMMAND}" "-fix" "-fix-errors")
            endif()
            set_target_properties(${target} PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_COMMAND}")
        endif()
    endif()
endfunction()

add_subdirectory(core)
add_subdirectory(tools)
add_subdirectory(engine)

# Add doxygen documentation

option(BUILD_DOCUMENTATION "Build docs" OFF)
if(BUILD_DOCUMENTATION)
    add_subdirectory(docs)
endif()

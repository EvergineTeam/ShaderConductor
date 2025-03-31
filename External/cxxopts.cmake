# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

set(cxxopts_REV "4bf61f08697b110d9e3991864650a405b3dd515d") # Updated in 2025-03-31 (v3.2.1)

UpdateExternalLib("cxxopts" "https://github.com/jarro2783/cxxopts.git" ${cxxopts_REV})

set(CXXOPTS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CXXOPTS_BUILD_TESTS OFF CACHE BOOL "" FORCE)

add_subdirectory(cxxopts EXCLUDE_FROM_ALL)

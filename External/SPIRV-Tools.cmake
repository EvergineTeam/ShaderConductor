# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

set(SPIRV_Tools_REV "393d5c7df150532045c50affffea2df22e8231b0") # Updated in 2025-03-28

UpdateExternalLib("SPIRV-Tools" "https://github.com/KhronosGroup/SPIRV-Tools.git" ${SPIRV_Tools_REV})

set(SPIRV_SKIP_EXECUTABLES OFF CACHE BOOL "" FORCE)
add_subdirectory(SPIRV-Tools EXCLUDE_FROM_ALL)
find_targets(targets SPIRV-Headers)
foreach(target ${targets})
    get_target_property(vsFolder ${target} FOLDER)
    if(NOT vsFolder)
        set(vsFolder "")
    endif()
    set_target_properties(${target} PROPERTIES FOLDER "External/SPIRV-Tools/${vsFolder}")
endforeach()

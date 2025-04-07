# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

set(SPIRV_Headers_REV "8c88e0c4c94a21de825efccba5f99a862b049825") # Updated in 2025-03-28

UpdateExternalLib("SPIRV-Headers" "https://github.com/KhronosGroup/SPIRV-Headers.git" ${SPIRV_Headers_REV})

add_subdirectory(SPIRV-Headers EXCLUDE_FROM_ALL)
find_targets(targets SPIRV-Headers)
foreach(target ${targets})
    set_target_properties(${target} PROPERTIES FOLDER "External/SPIRV-Headers")
endforeach()

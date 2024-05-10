# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

set(SPIRV_Headers_REV "1c6bb2743599e6eb6f37b2969acc0aef812e32e3")

UpdateExternalLib("SPIRV-Headers" "https://github.com/KhronosGroup/SPIRV-Headers.git" ${SPIRV_Headers_REV})

add_subdirectory(SPIRV-Headers EXCLUDE_FROM_ALL)
foreach(target
    "SPIRV-Headers")
    set_target_properties(${target} PROPERTIES FOLDER "External/SPIRV-Headers")
endforeach()

# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

set(SPIRV_Headers_REV "8b246ff75c6615ba4532fe4fde20f1be090c3764")

UpdateExternalLib("SPIRV-Headers" "https://github.com/KhronosGroup/SPIRV-Headers.git" ${SPIRV_Headers_REV})

add_subdirectory(SPIRV-Headers EXCLUDE_FROM_ALL)
foreach(target
    "install-headers" "SPIRV-Headers-example")
    set_target_properties(${target} PROPERTIES FOLDER "External/SPIRV-Headers")
endforeach()

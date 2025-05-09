# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

find_program(git_executable NAMES git git.exe git.cmd)
if(NOT git_executable)
    message(FATAL_ERROR "Failed to find git.")
endif()

function(UpdateExternalLib name url rev)
    set(need_checkout FALSE)
    set(external_folder "${SC_ROOT_DIR}/External")
    set(external_lib_folder "${external_folder}/${name}")
    if(EXISTS "${external_lib_folder}/.git")
        message(STATUS "Updating ${name} to revision ${rev}...")
        execute_process(COMMAND "${git_executable}" "fetch" "origin" WORKING_DIRECTORY "${external_lib_folder}")
        execute_process(COMMAND "${git_executable}" "rev-parse" "HEAD" WORKING_DIRECTORY "${external_lib_folder}" OUTPUT_VARIABLE head_rev)
        string(STRIP ${head_rev} head_rev)
        if (${head_rev} STREQUAL ${rev})
            set(need_checkout FALSE)
        else()
            set(need_checkout TRUE)
        endif()
    else()
        message(STATUS "Cloning ${name} revision...")
        execute_process(COMMAND "${git_executable}" "clone" "--recurse-submodules" ${url} "${external_lib_folder}")
        set(need_checkout TRUE)
    endif()
    if(need_checkout)
        message(STATUS "Checking out to revision ${rev}...")
        execute_process(COMMAND "${git_executable}" "checkout" "-q" ${rev} WORKING_DIRECTORY "${external_lib_folder}")
        execute_process(COMMAND "${git_executable}" "submodule" "update" "--recursive")
    endif()

    set(${ARGV3} ${need_checkout} PARENT_SCOPE)
endfunction()

function(ApplyPatch name patch)
    set(external_folder "${SC_ROOT_DIR}/External")
    set(external_lib_folder "${external_folder}/${name}")

    execute_process(COMMAND "${git_executable}" "apply" "--check" "--ignore-space-change" "${patch}" WORKING_DIRECTORY ${external_lib_folder} RESULT_VARIABLE checkout_err)
    if(NOT checkout_err)
        message(STATUS "Applying ${patch}...")
        execute_process(COMMAND "${git_executable}" "am" "--ignore-space-change" "${patch}" WORKING_DIRECTORY ${external_lib_folder})
    endif()
endfunction()

function(find_targets targets DIR)
    get_property(TGTS DIRECTORY "${DIR}" PROPERTY BUILDSYSTEM_TARGETS)
    foreach(TGT IN LISTS TGTS)
        #message(STATUS "Target: ${TGT}")
        set(targets ${targets} ${TGT})
    endforeach()

    get_property(SUBDIRS DIRECTORY "${DIR}" PROPERTY SUBDIRECTORIES)
    foreach(SUBDIR IN LISTS SUBDIRS)
        find_targets(targets "${SUBDIR}")
    endforeach()
endfunction()

include(cxxopts.cmake)
include(googletest.cmake)
include(SPIRV-Header.cmake)
include(SPIRV-Tools.cmake)
include(DirectXShaderCompiler.cmake)
include(SPIRV-Cross.cmake)

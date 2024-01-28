# project name
message("Working dir: ${CMAKE_CURRENT_SOURCE_DIR}")
get_filename_component(projectName ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" projectName ${projectName})
message("Project: ${projectName}")

# default build type
if(NOT DEFINED ${CMAKE_BUILD_TYPE})
    set(CMAKE_BUILD_TYPE Debug)
endif()

# create build folder
if(NOT DEFINED ${BUILD_DIR})
    set(BUILD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)
    STRING(REGEX REPLACE "\\\\" "/" BUILD_DIR ${BUILD_DIR})
    file(MAKE_DIRECTORY ${BUILD_DIR}/bin/)
endif()

message("[${projectName}] Build dir: ${BUILD_DIR}")

# tests
if(NOT DEFINED PSI_BUILD_TESTS)
    set(PSI_BUILD_TESTS true)
endif()

message("[${projectName}] PSI_BUILD_TESTS: ${PSI_BUILD_TESTS}")

# examples
if(NOT DEFINED PSI_BUILD_EXAMPLES)
    set(PSI_BUILD_EXAMPLES true)
endif()

message("[${projectName}] PSI_BUILD_EXAMPLES: ${PSI_BUILD_EXAMPLES}")

# create output folder
if(NOT DEFINED ${BUILD_OUT})
    set(BUILD_OUT ${BUILD_DIR}/bin/${CMAKE_BUILD_TYPE})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BUILD_OUT})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BUILD_OUT})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${BUILD_OUT})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${BUILD_OUT})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${BUILD_OUT})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${BUILD_OUT})
    file(MAKE_DIRECTORY ${BUILD_OUT})
endif()

message("[${projectName}] Build out: ${BUILD_OUT}")

# functions
if(NOT COMMAND find_submodule)
    function(find_submodule name path isDependent)
        get_filename_component(submodules_base_dir ${CMAKE_CURRENT_LIST_DIR} DIRECTORY BASE_DIR)

        # message ("submodules_base_dir: ${submodules_base_dir}/${name}")
        if(EXISTS ${submodules_base_dir}/${name})
            set(${path} ${submodules_base_dir}/${name} PARENT_SCOPE)
            set(${isDependent} "no" PARENT_SCOPE)
        elseif(EXISTS ${3rdPARTY_DIR}/${name})
            set(${path} ${3rdPARTY_DIR}/${name} PARENT_SCOPE)
            set(${isDependent} "yes" PARENT_SCOPE)
        else()
        endif()
    endfunction()
endif()

if(NOT COMMAND include_psi_dependency)
    function(include_psi_dependency name)
        find_submodule(psi-${name} dep_path is_dependent)
        message("[${projectName}] psi_${name}_dir: ${dep_path}, is_dependent: ${is_dependent}")

        if(NOT EXISTS ${dep_path})
            return()
        endif()

        include_directories(${dep_path}/psi/include)
    endfunction()
endif()

if(NOT COMMAND add_psi_dependency)
    function(add_psi_dependency name)
        find_submodule(psi-${name} dep_path is_dependent)
        message("[${projectName}] psi_${name}_dir: ${dep_path}, is_dependent: ${is_dependent}")

        if(NOT EXISTS ${dep_path})
            return()
        endif()

        include_directories(${dep_path}/psi/include)
        link_directories(${dep_path}/build/bin/${CMAKE_BUILD_TYPE})
        link_directories(${BUILD_OUT})

        file(GLOB_RECURSE psiLib1 ${dep_path}/build/bin/${CMAKE_BUILD_TYPE}/psi-${name}.lib)
        file(GLOB_RECURSE psiLib2 ${BUILD_OUT}/psi-${name}.lib)

        if(psiLib1 OR psiLib2)
            set(PSI_DEP_LIBS "${PSI_DEP_LIBS};psi-${name}" PARENT_SCOPE)
        else()
            if(${is_dependent} STREQUAL "yes")
                set(PSI_BUILD_TESTS false)
                set(PSI_BUILD_EXAMPLES false)

                message("[${projectName}] configuring submodule [psi-${name}]... ${dep_path}")
                add_subdirectory(${dep_path})
                set(PSI_DEP_LIBS "${PSI_DEP_LIBS};psi-${name}" PARENT_SCOPE)
            else()
                message("[${projectName}] If [psi-${name}] should not have library, then ignore this message. Otherwise, build it: ${dep_path}")
            endif()
        endif()

        if(${name} STREQUAL "logger")
            message("[${projectName}] found psi-logger")
            add_compile_definitions(PSI_LOGGER)
        endif()
    endfunction()
endif()

if(NOT COMMAND psi_make_tests)
    function(psi_make_tests name src libs)
        if(NOT ${PSI_BUILD_TESTS})
            return()
        endif()

        include_directories(${3rdPARTY_DIR}/psi-gtests/include)
        set(test_libs "gmock;gmock_main;gtest;gtest_main")
        link_directories(${3rdPARTY_DIR}/psi-gtests/lib/${SUB_DIR_LIBS})

        set(fileName PSI_TEST_${name})
        add_executable(${fileName} ${PROJECT_SOURCE_DIR}/tests/EntryPoint.cpp ${src})
        target_link_libraries(${fileName} ${libs} ${test_libs} ${PSI_DEP_LIBS})
    endfunction()
endif()

if(NOT COMMAND psi_make_examples)
    function(psi_make_examples name src libs)
        if(NOT ${PSI_BUILD_EXAMPLES})
            return()
        endif()

        set(fileName PSI_EXAMPLE_${name})
        add_executable(${fileName} ${src})
        target_link_libraries(${fileName} ${libs} ${PSI_DEP_LIBS})
    endfunction()
endif()
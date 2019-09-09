cmake_minimum_required(VERSION 3.15)

if(NOT TARGET CompileTimePlugins)
    project(CompileTimePlugins CXX)

    add_library(CompileTimePlugins INTERFACE)

    set(${PROJECT_NAME}_CONAN_LIBS)
    set(${PROJECT_NAME}_CONAN_REMOTES)

    list(APPEND ${PROJECT_NAME}_CONAN_LIBS "boost-di/1.1.0@inexorgame/stable")
    list(APPEND ${PROJECT_NAME}_CONAN_REMOTES "https://api.bintray.com/inexorgame/inexor-conan")

    include(cmake/AddConanLibs.cmake)
    AddConanLibs(TARGET ${PROJECT_NAME}
                 ACCESS INTERFACE
                 REMOTES ${${PROJECT_NAME}_CONAN_REMOTES}
                 LIBS ${${PROJECT_NAME}_CONAN_LIBS})
    
    set(INCLUDE_DIRS_TO_SEARCH_${CMAKE_BINARY_DIR}_PRE ${CMAKE_BINARY_DIR}/generated/include
                                                       ${CMAKE_CURRENT_SOURCE_DIR}/extern
                                                       ${CMAKE_CURRENT_SOURCE_DIR}/include)
    # get_target_property(INCLUDE_DIRS_TO_SEARCH_${CMAKE_BINARY_DIR}_PRE ${PROJECT_NAME} INCLUDE_DIRECTORIES)
    set(INCLUDE_DIRS_TO_SEARCH_${CMAKE_BINARY_DIR} "${INCLUDE_DIRS_TO_SEARCH_${CMAKE_BINARY_DIR}_PRE}"
        CACHE STRING "The directories to look for plugins in(at compiletimeplugins/plugins")
    set(INCLUDE_DIRS_TO_SEARCH ${INCLUDE_DIRS_TO_SEARCH_${CMAKE_BINARY_DIR}})
    
    set(COMPILETIMEPLUGINS_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
    set(COMPILETIMEPLUGINS_GENERATED_DIR ${CMAKE_BINARY_DIR}/generated)
    configure_file(cmake/generate.cmake.in ${CMAKE_BINARY_DIR}/generated/cmake/generateCompileTimePluginsHeader.cmake @ONLY)
    
    ADD_CUSTOM_TARGET(GenerateCompileTimePluginsHeader
                      COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/generated/cmake/generateCompileTimePluginsHeader.cmake
                      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                      BYPRODUCTS ${CMAKE_BINARY_DIR}/generated/include/compiletimeplugins/compiletimeplugins.h)
   
    message("TESTLOG RUNNING GENERATECMAKE")
    # We inline once so that we can guarantee the file exists so cmake doesn't freak out over missing source files.
    execute_process(COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/generated/cmake/generateCompileTimePluginsHeader.cmake
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

    target_sources(CompileTimePlugins INTERFACE ${CMAKE_BINARY_DIR}/generated/include/compiletimeplugins/compiletimeplugins.h
                                                include/compiletimeplugins/compiletimeregistry.h
                                                include/compiletimeplugins/utils.h)
    add_dependencies(${PROJECT_NAME} GenerateCompileTimePluginHeader)
endif()

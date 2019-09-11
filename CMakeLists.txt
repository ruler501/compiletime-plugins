cmake_minimum_required(VERSION 3.15)

if(NOT TARGET CompileTimePlugins)
    project(CompileTimePlugins
            VERSION 0.1.0
            DESCRIPTION "A library that assists in collecting a compile-time known set of plugins and serving them using [Boost].DI."
            HOMEPAGE_URL "https://github.com/ruler501/compiletime-plugins"
            LANGUAGES CXX)
    set(ENABLE_UNCONSTEXPR_TESTS OFF CACHE INTERNAL "Enable building the unconstexpr tests.")
    add_subdirectory(extern/unconstexpr)

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
    
    set(INCLUDE_DIRS_TO_SEARCH_PRE ${CMAKE_BINARY_DIR}/generated/include
                                   ${CMAKE_BINARY_DIR}/include
                                   ${CMAKE_CURRENT_SOURCE_DIR}/include)
    # Can't get INCLUDE_DIRECTORIES on an interface project
    # get_target_property(INCLUDE_DIRS_TO_SEARCH_PRE ${PROJECT_NAME} INCLUDE_DIRECTORIES)
    set(INCLUDE_DIRS_TO_SEARCH "${INCLUDE_DIRS_TO_SEARCH_PRE}"
        CACHE STRING "The directories to look for plugins in(at compiletimeplugins/plugins")
    set(COMPILETIMEPLUGINS_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
    set(COMPILETIMEPLUGINS_GENERATED_DIR ${CMAKE_BINARY_DIR}/generated)
    configure_file(cmake/generate.cmake.in ${CMAKE_BINARY_DIR}/generated/cmake/generateCompileTimePluginsHeader.cmake @ONLY)
    
    add_custom_command(COMMAND ${CMAKE_COMMAND} -P ${CMAKE_BINARY_DIR}/generated/cmake/generateCompileTimePluginsHeader.cmake
                       WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                       DEPENDS compiletimeplugins.h.in ${CMAKE_BINARY_DIR}/generated/cmake/generateCompileTimePluginsHeader.cmake
                       MAIN_DEPENDENCY compiletimeplugins.h.in
                       OUTPUT ${CMAKE_BINARY_DIR}/generated/include/compiletimeplugins/compiletimeplugins.h
                       BYPRODUCTS ${CMAKE_BINARY_DIR}/generated/include/compiletimeplugins/compiletimeplugins.h
                       VERBATIM)
    add_custom_target(GenerateCompileTimePluginsHeader
                      DEPENDS ${CMAKE_BINARY_DIR}/generated/include/compiletimeplugins/compiletimeplugins.h)
   
    set(INCLUDE_PLUGINS)
    # We inline once so that we can guarantee the file exists so cmake doesn't freak out over missing source files.
    configure_file(compiletimeplugins.h.in ${CMAKE_BINARY_DIR}/generated/include/compiletimeplugins/compiletimeplugins.h @ONLY)

    target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_BINARY_DIR}/generated/include include)
    target_sources(${PROJECT_NAME} INTERFACE ${CMAKE_BINARY_DIR}/generated/include/compiletimeplugins/compiletimeplugins.h
                                             include/compiletimeplugins/compiletimepluginsregistry.h
                                             include/compiletimeplugins/erasure.h)
    target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)
    target_link_libraries(${PROJECT_NAME} INTERFACE unconstexpr)
endif()

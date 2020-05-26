cmake_minimum_required(VERSION 3.15)

include_guard()
project(CompileTimePlugins
        VERSION 0.0.0
        DESCRIPTION "A library that assists in collecting a compile-time known set of plugins and serving them."
        HOMEPAGE_URL "https://github.com/ruler501/compiletime-plugins"
        LANGUAGES CXX)
add_library(CompileTimePlugins INTERFACE)

set(INCLUDE_DIRS_TO_SEARCH_PRE ${CMAKE_BINARY_DIR}/generated/include
                               ${CMAKE_BINARY_DIR}/include
                               ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(CompileTimePlugins PROPERTIES INCLUDE_DIRS_TO_SEARCH "${CMAKE_BINARY_DIR}/generated/include;${CMAKE_CURRENT_SOURCE_DIR}/include"
                                                    INCLUDE_FILES)

target_include_directories(CompileTimePlugins INTERFACE $<TARGET_PROPERTY:CompileTimePlugins,INCLUDE_DIRS_TO_SEARCH>)

file(GENERATE OUTPUT "${CMAKE_BINARY_DIR}/generated/include/compiletimeplugins/includedplugins.h"
              CONTENT "$<$<TARGET_PROPERTY:CompileTimePlugins,INCLUDE_PLUGINS>:#include \"$<JOIN:$<TARGET_PROPERTY:CompileTimePlugins,INCLUDE_PLUGINS>,\"\n#include \">\">"

target_sources(${PROJECT_NAME} INTERFACE ${CMAKE_BINARY_DIR}/generated/include/compiletimeplugins/includedplugins.h
                                         include/compiletimeplugins/erasure.h
                                         include/compiletimeplugins/compiletimeplugins.h)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)
target_link_libraries(${PROJECT_NAME} INTERFACE unconstexpr)

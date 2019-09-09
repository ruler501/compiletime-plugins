include(CMakeParseArguments)

macro(AddConanLibs)
    set(options)
    set(oneValueArgs TARGET ACCESS)
    set(multiValueArgs LIBS REMOTES)
    cmake_parse_arguments(ADD_CONAN "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT ADD_CONAN_LIBS)
        set(ADD_CONAN_LIBS ${${PROJECT_NAME}_CONAN_LIBS})
    endif()

    if(ADD_CONAN_LIBS AND (NOT ${ADD_CONAN_LIBS} STREQUAL NOTFOUND))
        if(NOT ADD_CONAN_ACCESS)
            set(ADD_CONAN_ACCESS PUBLIC)
        endif()
        if(NOT ADD_CONAN_TARGET)
            set(ADD_CONAN_TARGET ${PROJECT_NAME})
        endif()
        if(NOT ADD_CONAN_REMOTES)
            set(ADD_CONAN_REMOTES ${${PROJECT_NAME}_CONAN_REMOTES})
        endif()

        set(BUILD_FROM_SOURCE "" CACHE STRING "Build these packages from source, very time intensive. Can use all to make it build all from source.")

        # Download automatically, you can also just copy the conan.cmake file
        if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
            file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
                    "${CMAKE_BINARY_DIR}/conan.cmake")
        endif()

        include(${CMAKE_BINARY_DIR}/conan.cmake)

        conan_check(REQUIRED)
        if(DEFINED CONAN_COMMAND)
            set(GLOBAL PROPERTY CONAN_CMD "${CONAN_COMMAND}")
        endif()
        
        if(ADD_CONAN_REMOTES)
            foreach(_REMOTE ${ADD_CONAN_REMOTES})
                string(REPLACE "/" ";" _SPLIT_REMOTE ${_REMOTE})
                list(GET _SPLIT_REMOTE -2 _REMOTE_NAME)
                conan_add_remote(NAME ${_REMOTE_NAME} URL ${_REMOTE})
            endforeach()
        endif()
        
        if(BUILD_FROM_SOURCE)
            conan_cmake_run(REQUIRES ${ADD_CONAN_LIBS}
                            BUILD ${BUILD_FROM_SOURCE}
                            BASIC_SETUP CMAKE_TARGETS)
        else()
            conan_cmake_run(REQUIRES ${ADD_CONAN_LIBS}
                    BASIC_SETUP CMAKE_TARGETS)
        endif()
        foreach(_PACKAGE "${ADD_CONAN_LIBS}")
            string(REPLACE "/" ";" _SPLIT_PACKAGE ${_PACKAGE})
            list(GET _SPLIT_PACKAGE 0 _LIBRARY_NAME)
            target_link_libraries(${ADD_CONAN_TARGET} ${ADD_CONAN_ACCESS} "CONAN_PKG::${_LIBRARY_NAME}")
        endforeach()
    endif()
endmacro()

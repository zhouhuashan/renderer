cmake_minimum_required(VERSION 3.9)

project("renderer")

set(ORGANIZATION_NAME "My Company Name")
set(ORGANIZATION_SHORTNAME "mycompanyname")
set(ORGANIZATION_DOMAIN "mycompanyname.com")
set(PACKAGE_NAME "com.mycompanyname")

add_definitions(-DQT_NO_KEYWORDS)

#add_definitions(-DQT_NO_INFO_OUTPUT)
#add_definitions(-DQT_NO_DEBUG_OUTPUT)
#add_definitions(-DQT_NO_WARNING_OUTPUT)

execute_process(COMMAND git describe --always --tags --dirty --abbrev=40
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    OUTPUT_VARIABLE PROJECT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )

add_definitions(-DUNICODE=1 -DPROJECT_VERSION="${PROJECT_VERSION}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--gc-sections")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto -ffat-lto-objects")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -rdynamic -flto -ffat-lto-objects")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} -rdynamic -flto -ffat-lto-objects")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W")
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wsuggest-override")
else()
    message(FATAL_ERROR "Compiler ${CMAKE_CXX_COMPILER_ID} currently is not supported")
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG=1 -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb -fno-inline -DDEBUG=1 -march=x86-64 -mtune=generic")
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -D_GLIBCXX_DEBUG -D_LIBCPP_DEBUG2=1")
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og -D_GLIBCXX_DEBUG")
endif()
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -fno-omit-frame-pointer -DNDEBUG=1 -march=native")
if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -O3 -gline-tables-only")
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -Og -ggdb")
endif()
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} -Os -DNDEBUG=1 -march=native")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS YES)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BASE_DIRPATH}/bin/")
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BASE_DIRPATH}/lib/")
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BASE_DIRPATH}/lib/")

if(POLICY CMP0071)
  cmake_policy(SET CMP0071 NEW)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS
    Core
    LinguistTools
    Test
    Gui
    Widgets
    Concurrent
    Network
    WebSockets
    Qml
    Quick
    QuickControls2
    Multimedia
    3DAnimation
    3DCore
    3DExtras
    3DInput
    3DLogic
    3DQuick
    3DQuickAnimation
    3DQuickExtras
    3DQuickInput
    3DQuickRender
    3DQuickScene2D
    3DRender
    OpenGL
    DBus
    SerialPort
    RemoteObjects
    )

#[[
# QtCore and CUDA interference WRT -fPIC
# https://gitlab.kitware.com/cmake/cmake/issues/16915#note_270766
get_property(core_options TARGET Qt5::Core PROPERTY INTERFACE_COMPILE_OPTIONS)
string(REPLACE "-fPIC" "" new_core_options ${core_options})
set_property(TARGET Qt5::Core PROPERTY INTERFACE_COMPILE_OPTIONS ${new_core_options})
set_property(TARGET Qt5::Core PROPERTY INTERFACE_POSITION_INDEPENDENT_CODE "ON")
]]

function(ADD_TRANSLATION QM_FILES)
    foreach(TS_FILE ${ARGN})
        set(QM_FILE "${CMAKE_CURRENT_SOURCE_DIR}/${TS_FILE}.qm")
        add_custom_command(
            OUTPUT "${QM_FILE}"
            COMMAND "${Qt5_LRELEASE_EXECUTABLE}"
            ARGS -markuntranslated "Not translated!" -nounfinished -removeidentical -compress "${CMAKE_CURRENT_SOURCE_DIR}/${TS_FILE}.ts" -qm "${QM_FILE}"
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${TS_FILE}.ts" VERBATIM
        )
        list(APPEND ${QM_FILES} "${QM_FILE}")
    endforeach()
    set(${QM_FILES} ${${QM_FILES}} PARENT_SCOPE)
endfunction()

find_package(Threads)

# set OPENSSL_ROOT_DIR
set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL)

#target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::SSL OpenSSL::Crypto ${CMAKE_DL_LIBS} ${CMAKE_THREAD_LIBS_INIT})

find_package(PkgConfig REQUIRED)

function(ADD_PKG_MODULE PROJECT_NAME)
    foreach(PKG_NAME ${ARGN})
        pkg_search_module("${PKG_NAME}" REQUIRED "${PKG_NAME}")
        target_link_libraries(${PROJECT_NAME} PRIVATE ${${PKG_NAME}_LDFLAGS})
        target_include_directories(${PROJECT_NAME} PRIVATE ${${PKG_NAME}_INCLUDE_DIRS})
        target_compile_options(${PROJECT_NAME} PRIVATE ${${PKG_NAME}_CFLAGS})
    endforeach()
endfunction()

add_subdirectory("src")

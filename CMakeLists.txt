cmake_minimum_required(VERSION 3.0)
include(CheckCXXCompilerFlag)
include(FindPackageHandleStandardArgs)

project(bintracker)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

# set CMAKE_BUILD_TYPE to RELEASE if not set from command line
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RELEASE CACHE STRING "Choose the type of build, options are: DEBUG RELEASE" FORCE)
endif()


# find PugiXML
if(APPLE)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(PC_PUGIXML QUIET pugixml)
	set(PUGIXML_DEFINITIONS ${PC_PUGIXML_CFLAGS_OTHER})

	find_path(PUGIXML_INCLUDE_DIR pugixml.hpp HINTS ${PC_PUGIXML_INCLUDEDIR} ${PC_PUGIXML_INCLUDE_DIRS})
	find_library(PUGIXML_LIBRARIES NAMES pugixml HINTS ${PC_PUGIXML_LIBDIR} ${PC_PUGIXML_LIBRARY_DIRS})
else()
	find_path(PUGIXML_INCLUDE_DIR NAMES pugixml.hpp)
	find_library(PUGIXML_LIBRARIES NAMES pugixml)
endif()

find_package_handle_standard_args(PugiXML REQUIRED_VARS PUGIXML_INCLUDE_DIR PUGIXML_LIBRARIES)
mark_as_advanced(PUGIXML_INCLUDE_DIR PUGIXML_LIBRARIES)


# find libsamplerate
if(APPLE)
	find_package(PkgConfig REQUIRED)
	pkg_check_modules(PC_LIBSAMPLERATE QUIET libsamplerate)
	set(LIBSAMPLERATE_DEFINITIONS ${PC_LIBSAMPLERATE_CFLAGS_OTHER})

	find_path(LIBSAMPLERATE_INCLUDE_DIR samplerate.h HINTS ${PC_LIBSAMPLERATE_INCLUDEDIR} ${PC_LIBSAMPLERATE_INCLUDE_DIRS})
	find_library(LIBSAMPLERATE_LIBRARIES NAMES samplerate libsamplerate HINTS ${PC_LIBSAMPLERATE_LIBDIR} ${PC_LIBSAMPLERATE_LIBRARY_DIRS})
else()
    find_path(LIBSAMPLERATE_INCLUDE_DIR NAMES samplerate.h)
    find_library(LIBSAMPLERATE_LIBRARIES NAMES samplerate libsamplerate)
endif()
find_package_handle_standard_args(LIBSAMPLERATE REQUIRED_VARS LIBSAMPLERATE_INCLUDE_DIR LIBSAMPLERATE_LIBRARIES)
mark_as_advanced(LIBSAMPLERATE_INCLUDE_DIR LIBSAMPLERATE_LIBRARIES)


# find Allegro5
find_path(ALLEGRO5_INCLUDE_DIR allegro5/allegro.h
  /usr/local/include /usr/include $ENV{MINGDIR}/include ${CMAKE_SOURCE_DIR}/allegro/include)

set(ALLEGRO5_NAMES ${ALLEGRO5_NAMES} allegro liballegro Allegro-5.2 allegro-debug)
find_library(ALLEGRO5_LIBRARY NAMES ${ALLEGRO5_NAMES})
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ALLEGRO5 DEFAULT_MSG ALLEGRO5_LIBRARY ALLEGRO5_INCLUDE_DIR)

if(ALLEGRO5_FOUND)
    set(ALLEGRO5_LIBRARIES ${ALLEGRO5_LIBRARY})
    set(ALLEGRO5_LIBS ${ALLEGRO5_LIBS} ${ALLEGRO5_LIBRARIES})
else()
    message(FATAL_ERROR "Missing Allegro 5 library")
endif()

mark_as_advanced(ALLEGRO5_LIBRARY ALLEGRO5_INCLUDE_DIR)


# set directories and files
include_directories(. gui libmdal sound_emul sound_emul/machines
    sound_emul/machines/CPU ${ALLEGRO5_INCLUDE_DIR} ${LIBSAMPLERATE_INCLUDE_DIR} ${PUGIXML_INCLUDE_DIR})
if(NOT APPLE)
link_directories(${ALLEGRO5_LIBRARIES} ${PUGIXML_LIBRARIES} ${LIBSAMPLERATE_LIBRARIES})
endif()

set(SOURCES data_as/data_as.cpp
    gui/elements.cpp gui/events.cpp gui/file_io.cpp gui/gui.cpp gui/input.cpp
    gui/keyhandler.cpp gui/mousehandler.cpp gui/status.cpp gui/work_tune.cpp
    libmdal/blocks.cpp libmdal/command.cpp libmdal/config.cpp libmdal/field.cpp
    libmdal/module.cpp libmdal/sequence.cpp
    sound_emul/sound_emul.cpp sound_emul/machines/zxspectrum48.cpp
    sound_emul/machines/CPU/z80.cpp sound_emul/machines/CPU/z80instructions.cpp
    bintracker.cpp helper_func.cpp)

if(APPLE)
    set(AL_LIBS liballegro.dylib liballegro_main.dylib liballegro_image.dylib
        liballegro_font.dylib liballegro_primitives.dylib liballegro_dialog.dylib
        liballegro_audio.dylib)
    set(PUGIXML_LIB "${PUGIXML_LIBRARIES}")
    set(LIBSAMPLERATE_LIB "${LIBSAMPLERATE_LIBRARIES}")
else()
    set(AL_LIBS liballegro.so liballegro_main.so liballegro_image.so
        liballegro_font.so liballegro_primitives.so liballegro_dialog.so
        liballegro_audio.so)
    set(PUGIXML_LIB pugixml.so)
    set(LIBSAMPLERATE_LIB libsamplerate.so)
endif()



# Set flags. Say no to default CMAKE_CXX_FLAGS!
set(CMAKE_CXX_FLAGS "-std=c++11 -ffast-math")
if(NOT MINGW)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()
set(CMAKE_CXX_FLAGS_DEBUG "-g -pedantic -Wno-c++98-compat\
 -Wno-c++98-compat-pedantic -Wno-reserved-id-macro -Wno-header-hygiene\
 -Wno-padded -Wno-ignored-qualifiers -Wno-weak-vtables -Wno-global-constructors\
 -Wno-exit-time-destructors -Wno-double-promotion -Wno-conversion\
 -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
if(NOT APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
endif()
CHECK_CXX_COMPILER_FLAG(-Weverything HAVE_WEVERYTHING)
if (HAVE_WEVERYTHING)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Weverything")
else()
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
endif()

# check for libc++
CHECK_CXX_COMPILER_FLAG(-stdlib=libc++ HAVE_LIBCPP)
if((HAVE_LIBCPP) AND (NOT MINGW))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
endif()

# check for Address Sanitizer
if(WANT_ASAN)
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 4.1)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address" )
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
        else()
            message(WARNING "ASAN not available, check compiler support")
        endif()
    endif()
endif()


add_executable(bintracker ${SOURCES})
target_link_libraries(bintracker ${AL_LIBS} ${PUGIXML_LIB} ${LIBSAMPLERATE_LIB})

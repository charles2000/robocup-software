
# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11")

# Qt
set(CMAKE_AUTOMOC ON)

file(GLOB SOCCER_SRC
    "*.cpp"
    "config/*.cpp"
    "gameplay/*.cpp"
    "modeling/*.cpp"
    "motion/*.cpp"
    "planning/*.cpp"
    "radio/*.cpp"
    "joystick/*.cpp"
    )

# Exclude the two files that include a main() function - we'll add those in later for their respective executables
list(REMOVE_ITEM SOCCER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/LogViewer.cpp")
list(REMOVE_ITEM SOCCER_SRC "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

# help CMake find gl.h, glu.h, etc on OS X
if(APPLE)
    link_directories(/opt/X11/lib/)
    include_directories(/opt/X11/include/)
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories(${CMAKE_CURRENT_BINARY_DIR})

file(GLOB SOCCER_UIS "${CMAKE_CURRENT_SOURCE_DIR}/ui/*.ui")
qt5_wrap_ui(SOCCER_UIS ${SOCCER_UIS})

qt5_add_resources(SOCCER_RSRC ui/main_icons.qrc)



# Python
include_directories(${PYTHON_INCLUDE_DIRS})

# Boost Python
set(Boost_USE_STATIC_LIBS   OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost COMPONENTS system REQUIRED)

# find boost python
# This package is named 'python3' on Arch, 'python-py34' on Ubuntu 14, and 'python' on OS X
set(FOUND_A_BOOST_PYTHON FALSE)
foreach(possible_name python3 python-py32 python-py33 python-py34 python)
    find_package(Boost COMPONENTS ${possible_name})
    if (Boost_FOUND)
        set(FOUND_A_BOOST_PYTHON TRUE)
        break()
    endif()
endforeach()
if (NOT FOUND_A_BOOST_PYTHON)
    message(FATAL_ERROR "Unable to find a suitable version of boost python")
endif()

# libusb
include_directories(${LIBUSB_1_INCLUDE_DIRS})

# SDL
include_directories(${SDL_INCLUDE_DIRS})

# Build stand-alone soccer dylib
# This is linked into soccer and our unit tests, as well as being a python module
add_library(robocup SHARED ${SOCCER_SRC} ${SOCCER_UIS} ${SOCCER_RSRC})
set_target_properties(robocup PROPERTIES PREFIX "")

target_link_libraries(robocup common)
qt5_use_modules(robocup Widgets Xml Core OpenGL Network)
target_link_libraries(robocup ${LIBUSB_1_LIBRARIES})
target_link_libraries(robocup ${Boost_LIBRARIES})
target_link_libraries(robocup GL GLU glut)
target_link_libraries(robocup pthread)
target_link_libraries(robocup spnav)
target_link_libraries(robocup ${SDL_LIBRARY})

# python
# note: these are set in the root CMakeLists.txt file
include_directories(${PYTHON_INCLUDE_DIRS})
link_directories(${PYTHON_LINK_DIRS})
target_link_libraries(robocup ${PYTHON_LIBRARIES})

# 'soccer' program
if(APPLE)
    include(BundleUtilities)

    # app icon
    set(OSX_ICON_FILES ${CMAKE_CURRENT_SOURCE_DIR}/ui/SoccerBall.icns)
    set_source_files_properties(${OSX_ICON_FILES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set(MACOSX_BUNDLE_ICON_FILE SoccerBall)

    set(MACOSX_BUNDLE_GUI_IDENTIFIER "org.robojackets.robocup.soccer")

    add_executable(soccer MACOSX_BUNDLE main.cpp ${OSX_ICON_FILES})

    # add a script called "soccer" in the run/ dir that launches the soccer app and passes on all args,
    # so it can be used the same way the soccer executable is used on Linux
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/soccer
        "#!/bin/sh\n"
        "./soccer.app/Contents/MacOS/soccer $@")
    file(INSTALL ${CMAKE_CURRENT_BINARY_DIR}/soccer
        DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        FILE_PERMISSIONS OWNER_READ GROUP_READ OWNER_WRITE GROUP_WRITE OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)
else()
    add_executable(soccer main.cpp)
endif()
qt5_use_modules(soccer Widgets Xml Core OpenGL Network Svg)
target_link_libraries(soccer robocup)


# Unit tests
add_subdirectory(tests)


# build the 'log_viewer' program
qt5_add_resources(LOG_VIEWER_RSRC ui/log_icons.qrc)
qt5_wrap_ui(LOG_VIEWER_UI ui/LogViewer.ui)
set(LOG_VIEWER_SRC
    "LogViewer.cpp"
    "FieldView.cpp"
   	"ProtobufTree.cpp"
   	"StripChart.cpp"
)
add_executable(log_viewer ${LOG_VIEWER_SRC} ${LOG_VIEWER_UI} ${LOG_VIEWER_RSRC})
qt5_use_modules(log_viewer Core Widgets OpenGL Svg)
target_link_libraries(log_viewer common)

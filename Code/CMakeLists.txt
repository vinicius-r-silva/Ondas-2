cmake_minimum_required(VERSION 3.5)

project(TLine_Viewer LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb3")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(OpenCV REQUIRED)

add_executable(TLine_Viewer
  main.cpp
  tview.cpp
  tview.h
  tview.ui
  icons.qrc
  graph.cpp
  graph.h
  function.h
  function.cpp
)

target_link_libraries(TLine_Viewer PRIVATE Qt5::Widgets ${OpenCV_LIBS})

cmake_minimum_required(VERSION 3.17)
project(Filters)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(UNIX)
    if(EXISTS "$HOME/Qt/latest/gcc_64")
        set(CMAKE_PREFIX_PATH "$HOME/Qt/latest/gcc_64/lib/cmake")
    endif()
endif()

if(WIN32)
    if(EXISTS "C:/Qt/5.15/mingw81_64")
        set(CMAKE_PREFIX_PATH "C:/Qt/5.15/mingw81_64/lib/cmake")
    elseif(EXISTS "C:/Qt/5.15.2/mingw81_64")
        set(CMAKE_PREFIX_PATH "C:/Qt/5.15.2/mingw81_64/lib/cmake")
    endif()
endif()

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

add_executable(filters main.cpp filter.cpp)

target_link_libraries(filters Qt5::Core Qt5::Gui Qt5::Widgets)

cmake_minimum_required(VERSION 3.16)
project(StructureVisual)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Qt6组件
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# 启用Qt的MOC、UIC和RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 包含目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# 源文件
set(SOURCES
    main.cpp
    mainwindow.cpp
    model.cpp
    VisualItem.cpp
    GraphModel.cpp
    SortModel.cpp
    AlgorithmController.cpp
    simplestring.cpp
    test_dynamicarray.cpp
    test_hashmap.cpp
    test_simplestring.cpp
)

# 头文件
set(HEADERS
    mainwindow.h
    model.h
    VisualItem.h
    GraphModel.h
    SortModel.h
    AlgorithmController.h
    Graph.h
    HashMap.h
    IGraph.h
    Iterable.h
    Iterator.h
    ListGraph.h
    MatrixGraph.h
    dynamicarray.h
    simplestring.h
)

# UI文件
set(UI_FILES
    mainwindow.ui
)

# 创建主可执行文件
add_executable(StructureVisual ${SOURCES} ${HEADERS} ${UI_FILES})

# 链接Qt库
target_link_libraries(StructureVisual Qt6::Core Qt6::Widgets)

# 创建BFS演示程序
add_executable(BFSDemo demo_bfs.cpp ${HEADERS})
target_link_libraries(BFSDemo Qt6::Core Qt6::Widgets)

# 设置输出目录
set_target_properties(StructureVisual PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

set_target_properties(BFSDemo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 安装规则
install(TARGETS StructureVisual BFSDemo
    RUNTIME DESTINATION bin
)

# 编译选项
if(MSVC)
    target_compile_options(StructureVisual PRIVATE /W4)
    target_compile_options(BFSDemo PRIVATE /W4)
else()
    target_compile_options(StructureVisual PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(BFSDemo PRIVATE -Wall -Wextra -Wpedantic)
endif()

# 调试信息
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(StructureVisual PRIVATE DEBUG)
    target_compile_definitions(BFSDemo PRIVATE DEBUG)
endif()

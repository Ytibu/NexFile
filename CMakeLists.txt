cmake_minimum_required(VERSION 3.28)

# 项目名与语言类型声明
project(netDisk C)

# -------------------- 编译基础参数 --------------------
# 指定 C 标准与约束：要求使用 C11，不启用编译器扩展
# set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# 对齐 Makefile: -Wall -Wextra -g
add_compile_options(-Wall -Wextra -g)

# -------------------- 目录规划 --------------------
# 源码目录与二进制输出目录（均基于项目根目录）
set(SERVER_SRC_DIR "${CMAKE_SOURCE_DIR}/server")
set(CLIENT_SRC_DIR "${CMAKE_SOURCE_DIR}/client")
set(BIN_DIR "${CMAKE_SOURCE_DIR}/bin")

# 可执行文件输出到项目内 bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BIN_DIR}")

# 头文件搜索路径（对齐 Makefile 的 -Iinclude）
include_directories("${CMAKE_SOURCE_DIR}/include")

# -------------------- 源文件收集 --------------------
# 自动收集 server/client 目录下所有 .c 文件
file(GLOB SERVER_SRCS CONFIGURE_DEPENDS "${SERVER_SRC_DIR}/*.c")
file(GLOB CLIENT_SRCS CONFIGURE_DEPENDS "${CLIENT_SRC_DIR}/*.c")

# -------------------- 目标定义 --------------------
# server 目标：生成 bin/server.out
add_executable(server_app ${SERVER_SRCS})
set_target_properties(server_app PROPERTIES OUTPUT_NAME "server.out")

# client 目标：生成 bin/client.out
add_executable(client_app ${CLIENT_SRCS})
set_target_properties(client_app PROPERTIES OUTPUT_NAME "client.out")

target_compile_definitions(server_app PRIVATE _POSIX_C_SOURCE=200809L)
target_compile_definitions(client_app PRIVATE _POSIX_C_SOURCE=200809L)

# server 依赖线程库（对齐 Makefile 的 -lpthread）
# 使用 CMake 提供的线程目标，保证跨平台链接行为一致
find_package(Threads REQUIRED)
target_link_libraries(server_app PRIVATE Threads::Threads)

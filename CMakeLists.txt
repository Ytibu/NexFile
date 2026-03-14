# CMake 最低版本要求
cmake_minimum_required(VERSION 3.16)

# 项目名称与语言
project(NextFile C)

# C 标准配置：
# 使用 C11，并开启 GNU 扩展以兼容 strtok_r/strdup 等接口
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# 可执行文件统一输出到项目根目录下的 bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

# 全局编译选项：开启常用告警并保留调试信息
add_compile_options(-Wall -Wextra -g)

# 让 __FILE__ 等宏中的绝对路径显示为相对路径（GCC/Clang）
if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        "-fmacro-prefix-map=${CMAKE_SOURCE_DIR}/=."
        "-ffile-prefix-map=${CMAKE_SOURCE_DIR}/=."
    )
endif()

# 自动收集服务端源文件
file(GLOB SERVER_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/NexFile/server/src/*.c"
)

# 自动收集客户端源文件
file(GLOB CLIENT_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/NexFile/client/src/*.c"
)

# server 可执行文件与头文件路径
add_executable(server ${SERVER_SOURCES})
target_include_directories(server PRIVATE
    "${CMAKE_SOURCE_DIR}/NexFile/server/include"
    "${CMAKE_SOURCE_DIR}/NexFile/shared"
)
# server 依赖 pthread 线程库
target_link_libraries(server PRIVATE pthread crypt)

# client 可执行文件与头文件路径
add_executable(client ${CLIENT_SOURCES})
target_include_directories(client PRIVATE
    "${CMAKE_SOURCE_DIR}/NexFile/client/include"
    "${CMAKE_SOURCE_DIR}/NexFile/shared"
)
target_link_libraries(client PRIVATE crypt)

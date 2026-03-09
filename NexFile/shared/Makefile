# ============================================================
# netDisk 项目 Makefile（规范化版本）
# 目标：
# 1) 统一构建输出目录（不污染源码目录）
# 2) 支持 server/client 双目标构建
# 3) 自动生成并包含头文件依赖（.d）
# 4) 注释中文化，便于维护
# ============================================================

# -------------------- 工具链与基础参数 --------------------
CC       := gcc
CPPFLAGS := -Iinclude
CFLAGS   := -Wall -Wextra -g
LDFLAGS  :=

# -------------------- 目录规划（均在项目内） --------------------
SERVER_SRC_DIR := server
CLIENT_SRC_DIR := client

BUILD_DIR      := build
OBJ_DIR        := $(BUILD_DIR)/obj
DEP_DIR        := $(BUILD_DIR)/dep
BIN_DIR        := bin

# -------------------- 可执行文件命名 --------------------
SERVER_BIN := $(BIN_DIR)/server.out
CLIENT_BIN := $(BIN_DIR)/client.out

# -------------------- 源文件收集 --------------------
SERVER_SRCS := $(wildcard $(SERVER_SRC_DIR)/*.c)
CLIENT_SRCS := $(wildcard $(CLIENT_SRC_DIR)/*.c)

# 将源文件映射为目标文件与依赖文件
# 例如：server/tcpInit.c -> build/obj/server/tcpInit.o
SERVER_OBJS := $(patsubst %.c,$(OBJ_DIR)/%.o,$(SERVER_SRCS))
CLIENT_OBJS := $(patsubst %.c,$(OBJ_DIR)/%.o,$(CLIENT_SRCS))
SERVER_DEPS := $(patsubst %.c,$(DEP_DIR)/%.d,$(SERVER_SRCS))
CLIENT_DEPS := $(patsubst %.c,$(DEP_DIR)/%.d,$(CLIENT_SRCS))
DEPS        := $(SERVER_DEPS) $(CLIENT_DEPS)

# server 依赖线程库，client 当前无额外库依赖
LDLIBS_SERVER := -lpthread
LDLIBS_CLIENT :=

# 默认目标：同时构建 server 和 client
.PHONY: all
all: server client

# 便捷目标：单独构建 server/client
.PHONY: server client
server: $(SERVER_BIN)
client: $(CLIENT_BIN)

# 链接规则
$(SERVER_BIN): $(SERVER_OBJS) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS_SERVER) -o $@

$(CLIENT_BIN): $(CLIENT_OBJS) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS_CLIENT) -o $@

# 编译规则：
# -MMD -MP 用于自动生成依赖，写入对应 dep 目录
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(dir $@) $(dir $(DEP_DIR)/$*.d)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -MF $(DEP_DIR)/$*.d -c $< -o $@

# 目录创建规则（order-only 依赖）
$(BIN_DIR):
	@mkdir -p $@

# 清理规则：
# clean 只清理构建产物；distclean 额外清理 build 与 bin 目录
.PHONY: clean distclean
clean:
	rm -f $(SERVER_BIN) $(CLIENT_BIN) $(SERVER_OBJS) $(CLIENT_OBJS) $(DEPS)

distclean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# 帮助信息：查看可用目标
.PHONY: help
help:
	@echo "可用目标："
	@echo "  make / make all   - 构建 server 与 client"
	@echo "  make server       - 仅构建 server"
	@echo "  make client       - 仅构建 client"
	@echo "  make clean        - 清理目标文件与依赖文件"
	@echo "  make distclean    - 删除 build 与 bin 目录"

# 包含自动生成的依赖文件。
# 使用 '-' 前缀避免首次构建时因文件不存在报错。
-include $(DEPS)

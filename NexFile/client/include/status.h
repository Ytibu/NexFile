#ifndef __STATUS_H__
#define __STATUS_H__

#define MAX_PATH 1024

// 客户端需要维护的关键状态
typedef struct client_state {
    char current_dir[MAX_PATH];     // 当前工作目录
    char server_cwd[MAX_PATH];       // 服务端当前目录
    int  is_connected;               // 连接状态
    char username[64];                // 认证信息
} ClientState_t;

// 全局客户端状态：在 status.c 中定义，其他 .c 通过本声明直接使用。
extern ClientState_t g_clientState;

int initClientState(ClientState_t *state);
int updateClientState(ClientState_t *state, const char *new_dir);
void printClientState(const ClientState_t *state);

#endif
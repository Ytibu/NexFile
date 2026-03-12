#include "../include/status.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

ClientState_t g_clientState = {0};

int initClientState(ClientState_t *state)
{
    getcwd(state->current_dir, sizeof(state->current_dir));
    state->server_cwd[0] = '\0';
    state->is_connected = 0;
    state->username[0] = '\0';
    return 0;
}
int updateClientState(ClientState_t *state, const char *new_dir)
{
    if (state == NULL || new_dir == NULL)
    {
        return -1;
    }

    snprintf(state->server_cwd, sizeof(state->server_cwd), "%s", new_dir);
    return 0;
}
void printClientState(const ClientState_t *state)
{
    printf("Current Directory: %s\n", state->current_dir);
    printf("Server Directory: %s\n", state->server_cwd);
    printf("Connection Status: %s\n", state->is_connected ? "Connected" : "Disconnected");
    printf("Username: %s\n", state->username);
}
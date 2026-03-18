#ifndef __CLIENT_HANDLE_H__
#define __CLIENT_HANDLE_H__

#define MAX_PATH_LEN 1024

extern char clientPath[MAX_PATH_LEN];

int handle_authen_event(int clientFd);
void handleClient(int clientFd);

#endif // __CLIENT_HANDLE_H__
#include "windows_socket.h"
#include <winsock2.h>

int windows_socket_initialize()
{
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA wsaData;
    int err = WSAStartup(wVersionRequested, &wsaData);

    return err;
}

int windows_socket_cleanup()
{
    int result = WSACleanup();
    return result;
}

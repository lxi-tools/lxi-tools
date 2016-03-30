#include <stdio.h>
#include <string.h>
#include <lxi.h>

int main()
{
    char response[LXI_MESSAGE_LENGTH_MAX];
    int device, length, timeout = 1;
    char *command = "*IDN?";

    device = lxi_connect("10.42.0.42");

    lxi_send(device, command, strlen(command), timeout);
    lxi_receive(device, response, &length, timeout);

    printf("%s\n", response);

    lxi_disconnect(device);
}

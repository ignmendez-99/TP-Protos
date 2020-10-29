/**
 *   Test para probar el funcionamiento de netutils.c
 *   Este test es más amigable que el que hizo Juan
 */
#include <stdio.h>
#include <stdlib.h>
#include "netutils.h"

int
main(void) {

    // sockaddr_to_human() TEST

    char buff[50] = {0};
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port   = htons(9090),
    };
    addr.sin_addr.s_addr = htonl(0x01020304);
    const struct sockaddr *x = (const struct sockaddr *) &addr;

    for(uint8_t i = 0; i < 20; i++) {
        printf("%s\n", sockaddr_to_human(buff, i, x));
    }

    // lindo esto
    const uint8_t size = sizeof(buff)/sizeof(buff[0]);
    printf("-------\n%s\n", sockaddr_to_human(buff, size, x));


    

    return 0;
}
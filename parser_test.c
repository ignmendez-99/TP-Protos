/**
 *   Test para probar el funcionamiento de hello_parser.c y request_parser.c
 *   Este test es más amigable que el que hizo Juan
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "buffer.h"
#include "hello_parser.h"
#include "request_parser.h"


int
main(void) {

    // HELLO TEST 1

    buffer* buffer_hello = malloc(sizeof(buffer));
    uint8_t zona_hello[] = {0x05, 0x01, 0x00};
    buffer_init(buffer_hello, 100, zona_hello);
    buffer_write_adv(buffer_hello, sizeof(zona_hello));

    hello_parser *hp = malloc(sizeof(hello_parser));
    hello_parser_init(hp);

    enum hello_state hs = consume_hello_buffer(buffer_hello, hp);
    print_current_hello_parser(hp);


    // HELLO TEST 2

    buffer* buffer_hello_2 = malloc(sizeof(buffer));
    uint8_t zona_hello_2[] = {0x05, 0x00};
    buffer_init(buffer_hello_2, 100, zona_hello_2);
    buffer_write_adv(buffer_hello_2, sizeof(zona_hello_2));

    hello_parser *hp_2 = malloc(sizeof(hello_parser));
    hello_parser_init(hp_2);

    enum hello_state hs_2 = consume_hello_buffer(buffer_hello_2, hp_2);
    print_current_hello_parser(hp_2);


    // REQUEST TEST

    buffer* buffer_request = malloc(sizeof(buffer));
    uint8_t zona_request[] = {0x05, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x11, 0x11};
    buffer_init(buffer_request, 100, zona_request);
    buffer_write_adv(buffer_request, sizeof(zona_request));

    request_parser *rp = malloc(sizeof(request_parser));
    request_parser_init(rp);

    enum request_state rs = consume_request_buffer(buffer_request, rp);
    print_current_request_parser(rp);

    return 0;
}
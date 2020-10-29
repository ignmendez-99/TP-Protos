#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "buffer.h"
#include "hello_parser.h"
#include "request_parser.h"


int
main(void) {

    buffer* buffer_hello = malloc(sizeof(buffer));
    uint8_t zona_hello[] = {0x05, 0x01, 0x00};
    buffer_init(buffer_hello, 100, zona_hello);
    buffer_write_adv(buffer_hello, 3);

    buffer* buffer_request = malloc(sizeof(buffer));
    uint8_t zona_request[] = {0x05, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x11, 0x11};
    buffer_init(buffer_request, 100, zona_request);
    buffer_write_adv(buffer_request, sizeof(zona_request));


    hello_parser *hp = malloc(sizeof(hello_parser));
    hello_parser_init(hp);

    request_parser *rp = malloc(sizeof(request_parser));
    request_parser_init(rp);


    enum hello_state hs = consume_hello_buffer(buffer_hello, hp);
    print_current_hello_parser(hp);

    enum request_state rs = consume_request_buffer(buffer_request, rp);
    print_current_request_parser(rp);

    return 0;
}
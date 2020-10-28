#include <stdlib.h>
#include "hello_parser.h"


void 
hello_parser_init(hello_parser *hp) {
    hp->state = hello_reading_version;
    hp->methods_remaining = 0;
}

enum hello_state
consume_buffer(buffer *b, hello_parser *hp) {
    enum hello_state state = hp->state;  // Le damos un valor por si no se entra en el while

    while(buffer_can_read(b)) {
        const uint8_t c = buffer_read(b);
        state = hello_parser_feed(hp, c);
        if(state == hello_finished || state == hello_unsupported_version) {
            break;   // stop reading
        }
    }

    return state;
}

enum hello_state
parse_single_character(const uint8_t c, hello_parser *hp) {
    switch(hp->state) {

        case hello_reading_version:
            if(c == PROXY_SOCKS_HELLO_SUPPORTED_VERSION)
                hp->state = hello_reading_nmethods;
            else
                hp->state = hello_unsupported_version;
            break;
        
        case hello_reading_nmethods:
            if(c == 0) {
                // zero methods were given
                hp->state = hello_finished;
            } else {
                hp->methods_remaining = c;
                hp->state = hello_reading_methods;
            }
            break;
        
        case hello_reading_methods:
            // TODO: falta pensar esta parte
            break;
        
        case hello_finished:
        case hello_unsupported_version:
            // return these states now
            break;
            
        default:
            // Impossible state!
            exit(EXIT_FAILURE);
    }
    return hp->state;
}

int
hello_marshall(buffer *b, const uint8_t method) {

    size_t space_left_to_write;
    uint8_t *where_to_write_next = buffer_write_ptr(b, &space_left_to_write);
    if(space_left_to_write < SPACE_NEEDED_FOR_HELLO_MARSHALL) {
        return -1;
    }

    where_to_write_next[0] = PROXY_SOCKS_HELLO_SUPPORTED_VERSION;
    where_to_write_next[1] = method;
    buffer_write_adv(b, SPACE_NEEDED_FOR_HELLO_MARSHALL);
    return 0;
}
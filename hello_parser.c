#include <stdlib.h>
#include <stdio.h>   // TODO: borrar (solo para debuggear está)
#include "hello_parser.h"


void 
hello_parser_init(hello_parser *hp) {
    hp->state = hello_reading_version;
    hp->methods_remaining = 0;
    hp->methods_index = 0;
    hp->methods = NULL;
}

enum hello_state
consume_hello_buffer(buffer *b, hello_parser *hp) {
    enum hello_state state = hp->state;  // Le damos un valor por si no se entra en el while

    while(buffer_can_read(b)) {
        const uint8_t c = buffer_read(b);
        state = parse_single_hello_character(c, hp);
        if(state == hello_finished || state == hello_unsupported_version) {
            break;   // stop reading
        }
    }

    return state;
}

enum hello_state
parse_single_hello_character(const uint8_t c, hello_parser *hp) {
    switch(hp->state) {

        case hello_reading_version:
            if(c == PROXY_SOCKS_HELLO_SUPPORTED_VERSION)
                hp->state = hello_reading_nmethods;
            else
                hp->state = hello_unsupported_version;
            break;
        
        case hello_reading_nmethods:
            if(c <= 0) {
                // zero methods were given
                hp->state = hello_finished;
            } else {
                hp->methods_remaining = c;
                // TODO: hacerle free a esto  
                // TODO: catchear return error
                hp->methods = malloc(c);   
                hp->state = hello_reading_methods;
            }
            break;
        
        case hello_reading_methods:
            // TODO: falta ver que algún método que me envió el cliente lo soportemos 
            //       si no soportamos ninguno de los que nos mandó --> debemos retornar 0xFF
            hp->methods[hp->methods_index++] = c;
            if(hp->methods_index == hp->methods_remaining) {
                hp->state = hello_finished;
            }
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

    // TODO: nota solo para recordar: nuestro programa principal tendría 2 buffers, uno por cada flujo de
    //       datos (cliente-servidor y servidor-cliente). Por lo tanto, el buffer que recibe esta función
    //       seguro va a ser distinto que el buffer que recibieron las otras funciones de este archivo
    //       (ya que el otro buffer era leído, mientras que este de acá es escrito)

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

void
print_current_hello_parser(hello_parser *hp) {
    printf("/************** HELLO PARSER DATA **************/\n");
    printf("STATE = %d\n", hp->state);
    printf("NMETHODS = %d\n", hp->methods_remaining);
    printf("METHODS_ARRAY_INDEX = %d\n", hp->methods_index);
    for (uint8_t i = 0; i < hp->methods_remaining; i++){
        printf("METHOD[%d] = %d\n", i, hp->methods[i]);
    }
    printf("/***********************************************/\n");
}
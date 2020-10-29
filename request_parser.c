#include <stdlib.h>
#include <stdio.h>   // TODO: borrar (solo para debuggear está)
#include "request_parser.h"

// Prototypes
static reply * getReplyBasedOnState(enum request_state state);


void
request_parser_init(request_parser *rp) {
    rp->state = request_reading_version;
    rp->address_index = 0;
    rp->destination_port = -1;  // must store negative value to represent "first time checking this value"
    rp->destination_address = NULL;
    rp->reply = NULL;
}


enum request_state
consume_request_buffer(buffer *b, request_parser *rp) {
    enum request_state state = rp->state;  // Le damos un valor por si no se entra en el while

    while(buffer_can_read(b)) {
        const uint8_t c = buffer_read(b);
        state = parse_single_request_character(c, rp);
        if(state == request_finished) {
            // ejecutamos una vez más para generar el reply
            parse_single_request_character(c, rp);
            break;
        } else if(state == request_has_error) {
            break;   // stop reading
        }
    }

    return state;
}


enum request_state
parse_single_request_character(const uint8_t c, request_parser *rp) {
    switch(rp->state) {

        case request_reading_version:
            if(c == PROXY_SOCKS_REQUEST_SUPPORTED_VERSION)
                rp->state = request_reading_command;
            else {
                rp->state = request_has_error;
                rp->reply = getReplyBasedOnState(request_reading_version);
            }
            break;

        case request_reading_command:
            if(c == CONNECT_COMMAND)
                rp->state = request_reading_reserved;
            else {
                rp->state = request_has_error;
                rp->reply = getReplyBasedOnState(request_reading_command);
            }
            break;
        
        case request_reading_reserved:
            if(c == PROXY_SOCKS_REQUEST_RESERVED)
                rp->state = request_reading_address_type;
            else {
                rp->state = request_has_error;
                rp->reply = getReplyBasedOnState(request_reading_reserved);
            }
            break;
        
        case request_reading_address_type:
            if(c == REQUEST_THROUGH_IPV4) {
                rp->destination_address_length = 4;
                // TODO: hacer free() de esto
                // TODO: catchear NULL error
                rp->destination_address = malloc(4 + 1);  // +1 for \0
            } else if(c == REQUEST_THROUGH_IPV6) {
                rp->destination_address_length = 16;
                // TODO: hacer free() de esto
                // TODO: catchear NULL error
                rp->destination_address = malloc(16 + 1); // +1 for \0
            } else if(c == REQUEST_THROUGH_FQDN) {
                // Length is given in next state
            } else {
                rp->state = request_has_error;
                rp->reply = getReplyBasedOnState(request_reading_address_type);
                break;
            }
            
            rp->address_type = c;
            rp->state = request_reading_destination_address;
            break;

        case request_reading_destination_address:
            if(rp->address_index == 0 && rp->address_type == REQUEST_THROUGH_FQDN) {
                // reading first byte from FQDN
                rp->destination_address_length = c;
                // TODO: hacer free() de esto  
                // TODO: catchear posible NULL-return
                rp->destination_address = malloc(c + 1); // +1 for \0

            } else {
                // Save the byte
                rp->destination_address[rp->address_index++] = c;
                if(rp->address_index == rp->destination_address_length) {
                    // ya pusimos todos los bytes
                    rp->destination_address[rp->address_index++] = '\0';
                    rp->state = request_reading_destination_port;
                }
            }
            break;
        
        case request_reading_destination_port:
            if(rp->destination_port == -1){ 
                // first time here
                rp->destination_port = c * 256;
            } else {
                rp->destination_port += c;
                rp->state = request_finished;
            }
            break;
        
        case request_finished:
            rp->reply = getReplyBasedOnState(request_finished);
            break;
        
        case request_has_error:
            rp->reply = getReplyBasedOnState(request_has_error);
            break;
        
        default:
            // Impossible state!
            exit(EXIT_FAILURE);
    }
    return rp->state;
}

static reply * getReplyBasedOnState(enum request_state state) {
    reply *r = malloc(sizeof(reply));

    switch(state) {
        case request_reading_version:
        case request_reading_reserved:
        case request_has_error:
            r->code = 0x01;
            r->description = "General socks server failure";
            break;
        
        case request_reading_command:
            r->code = 0x07;
            r->description = "Command not supported";
            break;

        case request_reading_address_type:
            r->code = 0x08;
            r->description = "Address type not supported";
            break;

        case request_finished:
            r->code = 0x00;
            r->description = "Succeded";
            break;
    }
    return r;
}


int
request_marshall(buffer *b, const uint8_t method, request_parser *rp) {

    size_t space_left_to_write;
    uint8_t *where_to_write_next = buffer_write_ptr(b, &space_left_to_write);
    const uint16_t space_needed_for_request_marshall = 1 + 1 + 1 + 1 + rp->destination_address_length + 2;
    if(space_left_to_write < space_needed_for_request_marshall) {
        return -1;
    }

    where_to_write_next[0] = PROXY_SOCKS_REQUEST_SUPPORTED_VERSION;
    where_to_write_next[1] = rp->reply->code;
    where_to_write_next[2] = 0x00;  // RSV
    where_to_write_next[3] = rp->address_type;
    // where_to_write_next[4] =  TODO: dependiendo de lo que hizo el proxy, ponemos acá el BND.ADDR correspondiente
    // where_to_write_next[5] =  TODO: dependiendo de lo que hizo el proxy, ponemos acá el BND.PORT correspondiente
    return 0;
}


void
print_current_request_parser(request_parser *rp) {
    printf("/************** REQUEST PARSER DATA **************/\n");
    printf("STATE = %d\n", rp->state);
    printf("ADDRESS_TYPE = %d\n", rp->address_type);
    printf("DESTINATION_PORT = %ld\n", rp->destination_port);
    printf("DESTINATION_ADDRESS_LENGTH = %d\n", rp->destination_address_length);
    printf("DESTINATION_ADDRESS_INDEX = %d\n", rp->address_index);
    for (uint8_t i = 0; i < rp->destination_address_length; i++){
        printf("Request parser address[%d] = %d\n", i, rp->destination_address[i]);
    }
    printf("REPLY = {code: %d , message: %s}\n", rp->reply->code, rp->reply->description);
    printf("/*************************************************/\n");
}

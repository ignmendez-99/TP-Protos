#include <stdlib.h>
#include "request_parser.h"


void
request_parser_init(request_parser *rp) {
    rp->state = request_reading_version;
    rp->address_counter = 0;
    rp->destination_port = -1;  // must store negative value to represent "first time checking this value"
    rp->destination_address = NULL;
}


enum request_state
consume_buffer(buffer *b, request_parser *rp) {
    enum request_state state = rp->state;  // Le damos un valor por si no se entra en el while

    while(buffer_can_read(b)) {
        const uint8_t c = buffer_read(b);
        state = hello_parser_feed(rp, c);
        if(state == request_finished || state == request_has_error) {
            break;   // stop reading
        }
    }

    return state;
}


enum request_state
parse_single_character(const uint8_t c, request_parser *rp) {
    switch(rp->state) {

        case request_reading_version:
            if(c == PROXY_SOCKS_REQUEST_SUPPORTED_VERSION)
                rp->state = request_reading_command;
            else {
                rp->state = request_has_error;
                rp->reply = gen_socks_serv_fail_reply;
            }
            break;

        case request_reading_command:
            if(c == CONNECT_COMMAND)
                rp->state = request_reading_reserved;
            else {
                rp->state = request_has_error;
                rp->reply = cmd_not_supported_reply;
            }
            break;
        
        case request_reading_reserved:
            if(c == PROXY_SOCKS_REQUEST_RESERVED)
                rp->state = request_reading_address_type;
            else {
                rp->state = request_has_error;
                rp->reply = gen_socks_serv_fail_reply;
            }
            break;
        
        case request_reading_address_type:
            if(c == REQUEST_THROUGH_IPV4 || c == REQUEST_THROUGH_IPV6 || c == REQUEST_THROUGH_FQDN) {
                rp->address_type = c;
                rp->state = request_reading_destination_port;
            } else {
                rp->state = request_has_error;
                rp->reply = addr_type_not_supported_reply;
            }
            break;

        case request_reading_destination_address:
            switch(rp->address_type) {
                case REQUEST_THROUGH_IPV4:
                    rp->destination_address_length = 4;
                    break;
                case REQUEST_THROUGH_IPV6:
                    rp->destination_address_length = 16;
                    break;
                case REQUEST_THROUGH_FQDN:
                    if(rp->address_counter == 0) {
                        // fist time here. Must save address length
                        rp->destination_address_length = c;
                        rp->address_counter++;
                    }
                    break;
            }
            // Save the byte
            rp->destination_address[rp->address_counter++] = c;
            if(rp->address_counter == rp->destination_address_length) {
                // ya pusimos todos los bytes bytes
                rp->destination_address[rp->destination_address_length++] = '\0';
                rp->state = request_reading_destination_port;
            }
        
        case request_reading_destination_port:
            if(rp->destination_port == -1){ 
                // first time here
                rp->destination_port = (c << 8);   // c * 2^8  --> zero or 256 <= N < 65536
            } else {
                rp->destination_port += c;
                rp->state = request_finished;
            }
            break;
        
        case request_finished:
        case request_has_error:
            // return state now
            break;
        
        default:
            // Impossible state!
            exit(EXIT_FAILURE);
    }
    return rp->state;
}

int
request_marshall(buffer *b, const uint8_t method, request_parser *rp) {

    size_t space_left_to_write;
    uint8_t *where_to_write_next = buffer_write_ptr(b, &space_left_to_write);
    uint16_t space_needed_for_request_marshall = 1 + 1 + 1 + 1 + rp->destination_address_length + 2;
    if(space_left_to_write < space_needed_for_request_marshall) {
        return -1;
    }

    where_to_write_next[0] = PROXY_SOCKS_REQUEST_SUPPORTED_VERSION;

}
#ifndef REQUEST_PARSER_H_68f9cbe0499150288c6b905552e201fb15e0b420
#define REQUEST_PARSER_H_68f9cbe0499150288c6b905552e201fb15e0b420

#include "buffer.h"
#include <stdint.h>

#define PROXY_SOCKS_REQUEST_SUPPORTED_VERSION 0x05
#define PROXY_SOCKS_REQUEST_RESERVED          0x00   // Fields marked RESERVED (RSV) must be set to X'00'.

#define CONNECT_COMMAND 0x01

// Address Types
#define REQUEST_THROUGH_IPV4 0x01
#define REQUEST_THROUGH_FQDN 0x03
#define REQUEST_THROUGH_IPV6 0x04

#define REPLY_MAX_LENGTH 

typedef struct reply {
    uint8_t code;
     uint8_t *description;
} reply;


// Los posibles estados en los que se puede encontrar el parser de request
enum request_state {
    request_reading_version,
    request_reading_command,
    request_reading_reserved,
    request_reading_address_type,
    request_reading_destination_address,
    request_reading_destination_port,
    request_finished,
    request_has_error
};

enum request_address_types {
    request_IPv4,
    request_IPv6,
    request_FQDN
};


typedef struct request_parser {
    enum request_state state;
    uint8_t address_type;

    uint8_t *destination_address;
    uint8_t destination_address_length;
    ssize_t destination_port;   // must store negative value to represent "first time checking this value"
    uint8_t address_index;  // para ir metiendo los bytes de a uno en 'destination_address'

    reply *reply;
} request_parser;


/** inicializa las variables del parser */
void
request_parser_init(request_parser *rp);


/**
 * Dado un buffer, lo consume (lee) hasta que no puede hacerlo más (ya sea porque el buffer no permite leer más, 
 * o porque se llegó al estado 'request_finished')
 * Retorna el estado en que se encuentra el parser al terminar de consumir el buffer.
 */ 
enum request_state
consume_request_buffer(buffer *b, request_parser *rp);


/**
 * Parsea un caracter del buffer.
 * Retorna el estado en que se encuentra el parser al terminar de parsear el caracter.
 * 
 * Si devuelve el estado "request_has_error", entonces dejará un struct reply* en el campo "reply" del request_parser
 * con un código y mensaje de error apropiado.  
 */
enum request_state
parse_single_request_character( uint8_t c, request_parser *rp);


/**
 * Deja en el buffer la respuesta al request y retorna 0
 * También acomoda el puntero Write del buffer para que apunte a donde comienza la respuesta dejada.
 * 
 * En caso de que en el buffer no haya suficiente espacio para escribir la respuesta, retorna -1
 */
int
request_marshall(buffer *b,  uint8_t method, request_parser *rp);


/**   TODO: no se si esto va a hacer falta una vez que tengamos logging y esas cosas...
 * Imprime en pantalla el estado del request_parser
 */
void
print_current_request_parser(request_parser *rp);

#endif
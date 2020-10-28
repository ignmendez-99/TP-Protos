#ifndef REQUEST_PARSER_H  // TODO: poner un hash aca para que quede mas PRO
#define REQUEST_PARSER_H  // TODO: idem arriba

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
    const uint8_t *description;
} reply;
// Todos las posibles respuestas al Request
static const reply succeded_reply = {0x00, "Succeded"};
static const reply gen_socks_serv_fail_reply = {0x01, "General socks server failure"};
static const reply conn_not_allow_by_ruleset_reply = {0x02, "Connection not allowed by ruleset"};
static const reply net_unreachable_reply = {0x03, "Network unreachable"};
static const reply host_unreachable_reply = {0x04, "Host unreachable"};
static const reply conn_refused_reply = {0x05, "Connection refused"};
static const reply ttl_exp_reply = {0x06, "TTL expired"};
static const reply cmd_not_supported_reply = {0x07, "Command not supported"};
static const reply addr_type_not_supported_reply = {0x08, "Address type not supported"};


// enum reply_code {
//     SUCCEDED = 0x00,
//     GENERAL_SOCKS_SERVER_FAILURE = 0x01,
//     CONNECTION_NOT_ALLOWED_BY_RULESET = 0x02,
//     NETWORK_UNREACHABLE = 0x03,
//     HOST_UNREACHABLE = 0x04,
//     CONNECTION_REFUSED = 0x05,
//     TTL_EXPIRED = 0x06,
//     COMMAND_NOT_SUPPORTED = 0x07,
//     ADDRESS_TYPE_NOT_SUPPORTED = 0x08
// };


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
    uint8_t address_counter;  // para ir metiendo los bytes de a uno en 'destination_address'

    reply reply;
} request_parser;


/** inicializa el parser */
void
request_parser_init(request_parser *rp);


/**
 * Dado un buffer, lo consume (lee) hasta que no puede hacerlo más (ya sea porque llegó al final del buffer, 
 * o porque se llegó al estado 'request_finished')
 * Retorna el estado en que se encuentra el parser al terminar de consumir el buffer.
 */ 
enum request_state
consume_buffer(buffer *b, request_parser *rp);


/**
 * Parsea un caracter del buffer.
 * Retorna el estado en que se encuentra el parser al terminar de parsear el caracter.
 */
enum request_state
parse_single_character(const uint8_t c, request_parser *rp);


/**
 // TODO 
 */
int
request_marshall(buffer *b, const uint8_t method);

#endif
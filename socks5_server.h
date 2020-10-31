#ifndef SOCKS5_H_68f9cbe0499150288c6b905552e201fb15e0b420
#define SOCKS5_H_68f9cbe0499150288c6b905552e201fb15e0b420

#include "stm.h"
#include "buffer.h"

typedef struct socks5 {
    int client_fd;
    int server_fd;

    buffer *read_buffer;
    uint8_t *read_buffer_data; // Where 'read_buffer' will read from
    buffer *write_buffer;
    uint8_t *write_buffer_data; // Where 'write_buffer' will write to

    struct state_machine stm; // Gestor de máquinas de estado

} socks5;



/**
 * Ejecutada cada vez que el selector detecta que hay algo para leer en el socket pasivo del servidor
 * Acepta la nueva conexión entrante
 */
void
socksv5_passive_accept(struct selector_key *key);




#endif
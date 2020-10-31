#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "socks5_server.h"
#include "selector.h"
#include "stm.h"

#define BUFFER_SIZE 4096  // TODO: quizas conviene mover esto a otro archivo
                          // TODO: testear si es de tamaño adecuado


void
socksv5_passive_accept(struct selector_key *key) {

    struct sockaddr new_client_addr;
    const socklen_t new_client_addr_len = sizeof(new_client_addr);

    int client_sock = accept(key->fd, &new_client_addr, &new_client_addr_len);
    if(client_sock == -1) {
        goto finally;
    }

    // Definimos los handlers para el socket activo
    const struct fd_handler socksv5_active_handler = {
        .handle_read    = socks5_read,
        .handle_write   = socks5_write,
        .handle_block   = socks5_block,
        .handle_close   = socks5_close
    };

    // Registramos el nuevo socket activo en el mismo selector que mira al socket pasivo
    if(selector_register(key->s, client_sock, &socksv5_active_handler, OP_READ, state) != SELECTOR_SUCCESS) {
        goto finally;
    }


finally:
    // TODO: no te olvides de esta seccion
}

static struct
socks5 * create_new_socks5(int client_fd) {

    socks5 *s5 = calloc(1, sizeof(socks5));  // TODO: free this
    if(s5 == NULL) {
        return NULL;
    }

    state_machine *stm = calloc(1, sizeof(state_machine)); // TODO: free this
    if(stm == NULL) {
        return NULL;
    }
    stm->      ;
    s5->stm = *stm;

    // Initialize read_buffer
    buffer* rb = calloc(1, sizeof(buffer));  // TODO: free this
    if(rb == NULL) {
        return NULL;
    }
    buffer_init(rb, BUFFER_SIZE, s5->read_buffer_data);
    s5->read_buffer = rb;

    // Initialize write_buffer
    buffer *wb = calloc(1, sizeof(buffer)); // TODO: free this
    if(rb == NULL) {
        return NULL;
    }
    buffer_init(wb, BUFFER_SIZE, s5->write_buffer_data);
    s5->write_buffer = wb;

    s5->client_fd = client_fd;
    s5->server_fd = -1;  // Inválido por ahora
}


void
socks5_read(struct selector_key *key) {
    // TODO: hacer
}
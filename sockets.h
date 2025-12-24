#ifndef SOCKET_H
#define SOCKET_H

#include "types.h"
#include "panic_k.h"

#define MAX_RSOCKETS 10
#define MAX_SOCKETS 10
#define SOCKET_SIZE 256

typedef struct 
{
    uint8_t bound;
    uint8_t pid;
    uint8_t id;
    uint8_t read;
}socket_data;

extern uint32_t rsockets[MAX_RSOCKETS];
extern socket_data rsocket_info[MAX_RSOCKETS];

extern uint32_t sockets[SOCKET_SIZE * MAX_SOCKETS];
extern socket_data socket_info[MAX_RSOCKETS];

uint32_t read_rsocket(uint32_t socket_id);

void write_rsocket(uint32_t socket_id, uint32_t data);

uint32_t read_socket(uint32_t socket_id, uint32_t byte);

void write_socket(uint32_t socket_id, uint32_t byte, uint32_t data);

#endif

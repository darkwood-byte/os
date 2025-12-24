#include "sockets.h"

uint32_t rsockets[MAX_RSOCKETS] = {0};
socket_data rsocket_info[MAX_RSOCKETS] = {0};

uint32_t sockets[SOCKET_SIZE * MAX_SOCKETS] = {0};
socket_data socket_info[MAX_RSOCKETS] = {0};

uint32_t read_rsocket(uint32_t socket_id){
    if(socket_id >= MAX_RSOCKETS)k_panic("non vaild rsocket read, id: %d\n", socket_id);
     rsocket_info[socket_id].read = 1;
    return rsockets[socket_id];
}

void write_rsocket(uint32_t socket_id, uint32_t data){
    if(socket_id >= MAX_RSOCKETS)k_panic("non vaild rsocket write, id: %d\n", socket_id);
    rsockets[socket_id] = data;
     rsocket_info[socket_id].read = 0;
}

uint32_t read_socket(uint32_t socket_id, uint32_t byte){
    if(socket_id >= MAX_RSOCKETS)k_panic("non vaild socket read, id: %d\n", socket_id);
    if(byte >= SOCKET_SIZE)k_panic("non vailid socket reading size sellected, byte: %d", byte);
    socket_info[socket_id].read = 1;
    return sockets[socket_id * SOCKET_SIZE + byte];
}

void write_socket(uint32_t socket_id, uint32_t byte, uint32_t data){
    if(socket_id >= MAX_RSOCKETS)k_panic("non vaild socket write, id: %d\n", socket_id);
    if(byte >= SOCKET_SIZE)k_panic("non vailid socket writing size sellected, byte: %d", byte);
    sockets[socket_id * SOCKET_SIZE + byte] = data;
    socket_info[socket_id].read = 0;
}
//claiming
uint32_t claim_rsocket(uint32_t rsocket_id, uint32_t id){
    if(rsocket_id >= MAX_RSOCKETS)k_panic("non vaild rsocket claim, id: %d\n", rsocket_id);
    if(rsocket_info[rsocket_id].bound == 0){
        rsocket_info[rsocket_id].bound = 1;
        rsocket_info[rsocket_id].id = id;//indenifier
        rsocket_info[rsocket_id].pid = currproc->pid;//owner
        return 1;
    }
    else return 0;//could not bind rsocket
}  

uint32_t claim_socket(uint32_t socket_id, uint32_t id){
    if(socket_id >= MAX_RSOCKETS)k_panic("non vaild socket claim, id: %d\n", socket_id);
    if(socket_info[socket_id].bound == 0){
        socket_info[socket_id].bound = 1;
        socket_info[socket_id].id = id;//indenifier
        socket_info[socket_id].pid = currproc->pid;//owner
        return 1;
    }
    else return 0;//could not bind socket
}  

void unclaim_rsocket(uint32_t rsocket_id){
    if(rsocket_id >= MAX_RSOCKETS)k_panic("non vaild rsocket unclaim, id: %d\n", rsocket_id);
    rsocket_info[rsocket_id].bound = 0;
}

void unclaim_socket(uint32_t socket_id){
    if(socket_id >= MAX_RSOCKETS)k_panic("non vaild rsocket unclaim, id: %d\n", socket_id);
    rsocket_info[socket_id].bound = 0;
}
//info
uint32_t info_rsocket(uint32_t rsocket_id){
    if(rsocket_id >= MAX_RSOCKETS)k_panic("non vaild rsocket info get, id: %d\n", rsocket_id);
    return ((uint32_t)rsocket_info[rsocket_id].bound << 24) | ((uint32_t)rsocket_info[rsocket_id].pid << 16) | ((uint32_t)rsocket_info[rsocket_id].id << 8) |(uint32_t)rsocket_info[rsocket_id].read;
}

uint32_t info_socket(uint32_t socket_id){
    if(socket_id >= MAX_RSOCKETS)k_panic("non vaild socket info get, id: %d\n", socket_id);
    return ((uint32_t)rsocket_info[socket_id].bound << 24) | ((uint32_t)rsocket_info[socket_id].pid << 16) | ((uint32_t)rsocket_info[socket_id].id << 8) |(uint32_t)rsocket_info[socket_id].read;
}

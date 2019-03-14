#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "costs.h"
#include "logger.h"
#include "machine.h"
#include "receiver.h"
#include "sender.h"

/**
 * @brief Receiver thread (Thread #1)
 * 
 * Repeats forever, receiving messages from
 * other nodes and updates the neighbor cost
 * table.  Updates cost in both directions,
 * cost c from x to y and y to x.
 * 
 * @param _cfg   Configuration
 * @return void*
 */
void* run_receiver(void* _cfg) {
    Config cfg = *(Config*) _cfg;
    int sock;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;

    // setup server
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port   = htons((short) cfg.machine->port);
    inet_pton(AF_INET, cfg.machine->ip, &(serverAddr.sin_addr.s_addr));

    memset((char *)serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));  
    addr_size = sizeof(serverStorage);

    // create socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        log_error("socket error");
        return 0;
    }

    // bind
    if (bind(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) != 0) {
        log_error("bind error");
        return 0;
    }

    log_info("Machine [%d] %s listening on %s:%d",
        cfg.machine->id, cfg.machine->name, cfg.machine->ip, cfg.machine->port);

    CostTableCopy* msg_copy = malloc(sizeof(CostTableCopy));
    CostTable* msg = malloc(sizeof(CostTable));
    
    while(1) {
        log_debug("waiting for message");
        log_debug("%zd", sizeof(*msg_copy));
        size_t res = recvfrom(sock, msg_copy, sizeof(*msg_copy), 0, (struct sockaddr *)&serverStorage, & addr_size);
        log_debug("received message size %zd, updating costs", res);

        log_debug("test %d", msg_copy->table[0][0]);

        msg = cost_table_from_copy(msg_copy);

        update_costs(cfg.costs, msg);
        log_debug("updated costs, printing costs");
        print_costs(cfg.costs);
        log_debug("trigger update to neighbors");
        receive_update(&cfg, msg);
    }
    return 0;
}

#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "costs.h"
#include "sender.h"
#include "logger.h"
#include "machine.h"

/**
 * @brief Reads new cost changes from stdin every 10 seconds
 * 
 * @param cfg Configuration
 */
void read_changes(Config* cfg) {
    // loop only 2 times?
    for (int i = 0; i < 2; ++i) {
        sleep(10);

        int target;
        int new_cost;
        printf("Enter <id> <new_cost>(Current Machine: %d):  ", cfg->machine->id);
        fflush(stdout);

        scanf("%d %d", &target, &new_cost);

        if (target > 3 || target < 0) {
            log_error("invalid machine id");
            continue;
        }

        log_info("updating machine %d <-> %d with new cost %d",
            cfg->machine->id, target, new_cost);

        // source, start at hop count 0
        // (increments in receive_update so start at -1 here)
        cfg->costs->hop_count = -1;
        receive_update(cfg, cfg->costs);
    }

    // sleep for another 30 seconds ?
    sleep(30);
}


void receive_update(Config* cfg, CostTable* cost_table) {

    // check if hop limit exceeded
    if (cfg->costs->hop_count > 8) {
        return;
    }

    // increase hop count
    cfg->costs->hop_count += 1;
    // update_costs(cfg->costs, cfg->costs);
    
    send_costs(cfg, cfg->costs);
}

/**
 * @brief Sends a cost update message to a single machine
 * 
 * @param target Machine to send the message to
 * @param msg    Message to send
 * @return int   1 if success, 0 if fail
 */
int send_cost(Machine* target, CostTable* cost_table) {
    int sock;
    struct sockaddr_in serverAddr;
    socklen_t addr_size;

    // configure address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port   = htons(target->port);
    inet_pton(AF_INET, target->ip, &serverAddr.sin_addr.s_addr);
    memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));  
    addr_size = sizeof serverAddr;

    // create udp socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        log_error("socket error sending to machine #%d (hop count %d)",
            target->id, cost_table->hop_count);
        return 0;
    }

    log_info("sending message to machine #%d (hop count %d)",
        target->id, cost_table->hop_count);

    if (sendto(sock, cost_table, sizeof(CostTable), 0,
        (struct sockaddr*) &serverAddr, addr_size) == -1) {
            log_error("failed sending message to machine %d (hop count %d)",
                target->id, cost_table->hop_count);
            return 0;
    }

    return 1;
}

/**
 * @brief Sends a cost update message to all other nodes
 * 
 * @param cfg Configuration with machine data
 * @param cost_table Message to send
 */
void send_costs(Config* cfg, CostTable* cost_table) {

    // TODO: costs should only send table to neighbor nodes
    // use received costtable's hop count + 1

    Machine* machines = cfg->machines;
    int curr_id = cfg->machine->id;
    // send the message to all **other** nodes
    for (int i = 0; i < 4; ++i) {
        // skip current machine
        if (i == curr_id) {
            continue;
        }

        size_t** table = lock_table(cost_table);
        // skip machines that aren't neighbors
        if (table[i][curr_id] >= 100) {
            continue;
        }

        send_cost(&machines[i], cost_table);

        // unlock after sending
        unlock_table(cost_table);
    }
}

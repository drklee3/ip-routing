#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
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
        printf("Enter <id> <new_cost>: ");
        fflush(stdout);

        scanf("%d %d", &target, &new_cost);

        if (target > 3 || target < 0) {
            log_error("invalid machine id");
            continue;
        }

        log_info("updating machine %d <-> %d with new cost %d",
            cfg->machine->id, target, new_cost);
        
        // create message format
        int msg[] = {cfg->machine->id, target, new_cost};
        update_costs(cfg->costs, msg);
        
        send_costs(cfg, msg);
    }

    // sleep for another 30 seconds ?
    sleep(30);
}

/**
 * @brief Sends a cost update message to a single machine
 * 
 * @param target Machine to send the message to
 * @param msg    Message to send
 * @return int   1 if success, 0 if fail
 */
int send_cost(Machine* target, int* msg) {
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
        log_error("socket error sending [%d %d %d]",
            msg[0], msg[1], msg[2]);
        return 0;
    }

    log_info("sending message [%d %d %d] to %d",
        msg[0], msg[1], msg[2], target->id);

    // message size is 3 * int
    if (sendto(sock, msg, 3 * sizeof(int), 0,
        (struct sockaddr*) &serverAddr, addr_size) == -1) {
            log_error("failed sending [%d %d %d]",
                msg[0], msg[1], msg[2]);
            return 0;
    }

    return 1;
}

/**
 * @brief Sends a cost update message to all other nodes
 * 
 * @param cfg Configuration with machine data
 * @param msg Message to send
 */
void send_costs(Config* cfg, int* msg) {
    Machine* machines = cfg->machines;
    int curr_id = cfg->machine->id;
    // send the message to all **other** nodes
    for (int i = 0; i < 4; ++i) {
        // skip current machine
        if (i == curr_id) {
            continue;
        }

        send_cost(&machines[i], msg);
    }
}

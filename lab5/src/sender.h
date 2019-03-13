#ifndef SENDER_H
#define SENDER_H

#include "costs.h"
#include "machine.h"

void read_changes(Config* cfg);
void receive_update(Config* cfg, CostTable* cost_table);
void send_costs(Config* cfg, CostTable* cost_table);

#endif

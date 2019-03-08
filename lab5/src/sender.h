#ifndef SENDER_H
#define SENDER_H

#include "machine.h"

void read_changes(Config* cfg);
void send_costs(Config* cfg, int* msg);

#endif

// fragmentation.h
#ifndef FRAGMENTATION_H
#define FRAGMENTATION_H

#define MAX_NODES 10

void fragment_and_route(char *payload, int mtu, int source, int destination, 
                        int topology[MAX_NODES][MAX_NODES], int num_nodes);

#endif
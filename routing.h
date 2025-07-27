#ifndef ROUTING_H
#define ROUTING_H

#define MAX_NODES 10

int findShortestPath(int source, int dest, int topology[MAX_NODES][MAX_NODES], 
                     int num_nodes, int path[MAX_NODES]);

#endif
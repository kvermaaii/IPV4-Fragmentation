#include <stdio.h>
#include <limits.h>
#include "routing.h"

#define INF INT_MAX

int findShortestPath(int source, int dest, int topology[MAX_NODES][MAX_NODES], int num_nodes, int path[MAX_NODES]) {
    int dist[MAX_NODES], visited[MAX_NODES], prev[MAX_NODES];
    for (int i = 0; i < num_nodes; i++) {
        dist[i] = INF;
        visited[i] = 0;
        prev[i] = -1;
        path[i] = -1; 
    }
    dist[source] = 0;
    for (int count = 0; count < num_nodes; count++) {
        int u = -1;
        for (int i = 0; i < num_nodes; i++) {
            if (!visited[i] && (u == -1 || dist[i] < dist[u]))
                u = i;
        }
        if (u == -1 || dist[u] == INF)
            break;
        visited[u] = 1;
        for (int v = 0; v < num_nodes; v++) {
            if (!visited[v] && topology[u][v] > 0 && 
                dist[u] != INF && dist[u] + topology[u][v] < dist[v]) {
                dist[v] = dist[u] + topology[u][v];
                prev[v] = u;
            }
        }
    }
    if (dist[dest] == INF) {
        return -1;
    }
    int index = 0;
    for (int at = dest; at != -1; at = prev[at]) {
        path[index++] = at;
    }
    for (int i = 0; i < index / 2; i++) {
        int temp = path[i];
        path[i] = path[index - 1 - i];
        path[index - 1 - i] = temp;
    }
    return dist[dest]; 
}
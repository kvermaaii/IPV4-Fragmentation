#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "routing.h"
#include "fragmentation.h"

#define MAX_PAYLOAD_SIZE 2048

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int get_valid_int(const char *prompt, int min, int max) {
    int value;
    char buffer[100];
    
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin)) {
            if (sscanf(buffer, "%d", &value) == 1) {
                if (value >= min && value <= max) {
                    return value;
                }
            }
        }
        printf("Invalid input. Please enter a number between %d and %d.\n", min, max);
    }
}

void initialize_topology(int topology[MAX_NODES][MAX_NODES], int *num_nodes) {
    *num_nodes = get_valid_int("Enter the number of nodes (2-10): ", 2, MAX_NODES);
    
    printf("\nEnter the adjacency matrix for the topology:\n");
    printf("(Enter weight for each link, 0 means no direct connection)\n\n");
    
    printf("    ");
    for (int i = 0; i < *num_nodes; i++) {
        printf("%3d ", i);
    }
    printf("\n");
    printf("    ");
    for (int i = 0; i < *num_nodes; i++) {
        printf("--- ");
    }
    printf("\n");
    
    for (int i = 0; i < *num_nodes; i++) {
        printf("%2d | ", i);
        for (int j = 0; j < *num_nodes; j++) {
            if (i == j) {
                topology[i][j] = 0; 
                printf("  0 ");
            } else {
                char prompt[64];
                sprintf(prompt," "); 
                topology[i][j] = get_valid_int("", 0, 999);
                printf("%3d ", topology[i][j]);
            }
        }
        printf("\n");
    }
}

// Main function
int main() {
    int source, destination, mtu;
    char payload[MAX_PAYLOAD_SIZE];
    int topology[MAX_NODES][MAX_NODES] = {0};
    int num_nodes;
    
    printf("\n\033[1m==== IPv4 Fragmentation and Routing Simulator ====\033[0m\n\n");
    
    initialize_topology(topology, &num_nodes);
    
    source = get_valid_int("\nEnter source node (0 to n-1): ", 0, num_nodes - 1);
    destination = get_valid_int("Enter destination node (0 to n-1): ", 0, num_nodes - 1);
    
    while (source == destination) {
        printf("Error: Source and destination cannot be the same node.\n");
        destination = get_valid_int("Enter destination node (0 to n-1): ", 0, num_nodes - 1);
    }
    
    mtu = get_valid_int("Enter MTU size (68-1500): ", 68, 1500);
    
    printf("Enter payload data: ");
    fgets(payload, MAX_PAYLOAD_SIZE, stdin);
    
    size_t len = strlen(payload);
    if (len > 0 && payload[len - 1] == '\n') {
        payload[len - 1] = '\0';
    }
    
    fragment_and_route(payload, mtu, source, destination, topology, num_nodes);
    
    return 0;
}

/*
0 7 12 0 0 0
7 0 2 9 0 0
12 2 0 0 10 0
0 9 0 0 4 1
0 0 10 4 0 5
0 0 0 1 5 0

7 12 0 0 0
7 2 9 0 0
12 2 0 10 0
0 9 0 4 1
0 0 10 4 5
0 0 0 1 5 
*/
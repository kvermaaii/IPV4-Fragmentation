#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "fragmentation.h"
#include "routing.h"

#define IP_HEADER_SIZE 20 
#define FRAGMENT_UNIT 8    

// IPv4 header structure
typedef struct {
    unsigned int version:4;         // IP version (4)
    unsigned int ihl:4;             // Internet Header Length in 32-bit words
    unsigned int dscp:6;            // Differentiated Services Code Point
    unsigned int ecn:2;             // Explicit Congestion Notification
    unsigned int total_length:16;   // Total packet length in bytes
    unsigned int identification:16; // Unique packet ID
    unsigned int reserved:1;        // Reserved bit (always 0)
    unsigned int df:1;              // Don't Fragment flag
    unsigned int mf:1;              // More Fragments flag
    unsigned int fragment_offset:13; // Fragment offset in 8-byte units
    unsigned int ttl:8;             // Time To Live
    unsigned int protocol:8;        // Protocol (TCP, UDP, etc.)
    unsigned int header_checksum:16; // Header checksum
    unsigned int source_ip:32;      // Source IP address
    unsigned int dest_ip:32;        // Destination IP address
} IPv4Header;

//initialize the IPv4 header 
void init_ipv4_header(IPv4Header *header) {
    header->version = 4;            
    header->ihl = 5;            
    header->dscp = 0;              
    header->ecn = 0;                
    header->total_length = 0;      
    header->identification = 0;     
    header->reserved = 0;          
    header->df = 0;                 
    header->mf = 0;                
    header->fragment_offset = 0;   
    header->ttl = 64;               
    header->protocol = 17;          
    header->header_checksum = 0;   
    header->source_ip = 0;         
    header->dest_ip = 0;           
}

void print_path_details(int path[MAX_NODES], int num_nodes, int path_length) {
    int path_count = 0;
    
    // Count actual path elements
    while (path_count < num_nodes && path[path_count] != -1) {
        path_count++;
    }
    
    printf("  \033[1mShortest Path:\033[0m ");
    if (path_count <= 0) {
        printf("No valid path found!\n");
        return;
    }
    
    for (int i = 0; i < path_count; i++) {
        printf("%d", path[i]);
        if (i < path_count - 1) {
            printf(" -> ");
        }
    }
    printf(" (Total cost: %d)\n", path_length);
    
    printf("  \033[1mRouting Details:\033[0m\n");
    for (int i = 0; i < path_count - 1; i++) {
        int from = path[i];
        int to = path[i + 1];
        printf("    Hop %d: Node %d -> Node %d\n", i + 1, from, to);
    }
}

void fragment_and_route(char *payload, int mtu, int source, int destination, 
                       int topology[MAX_NODES][MAX_NODES], int num_nodes) {
    if (!payload || mtu <= IP_HEADER_SIZE || source < 0 || destination < 0 || 
        source >= num_nodes || destination >= num_nodes) {
        fprintf(stderr, "Error: Invalid parameters for fragmentation.\n");
        return;
    }
    
    int payload_length = strlen(payload);
    int max_data_size = mtu - IP_HEADER_SIZE;  // Maximum data per fragment
    
    if (max_data_size <= 0) {
        fprintf(stderr, "Error: MTU too small for valid IPv4 packets.\n");
        return;
    }
    
    srand(time(NULL));
    unsigned short packet_id = rand() % 65536;
    
    printf("\n\033[1m==== IPv4 Fragmentation ====\033[0m\n");
    printf("Source: %d, Destination: %d\n", source, destination);
    printf("Original payload size: %d bytes\n", payload_length);
    printf("MTU: %d bytes (max data per fragment: %d bytes)\n", mtu, max_data_size);
    printf("Packet ID: %u\n", packet_id);
    
    int total_fragments = (payload_length + max_data_size - 1) / max_data_size;
    printf("Total fragments needed: %d\n\n", total_fragments);
    
    printf("\033[1m---- Fragment Details ----\033[0m\n");
    
    int offset = 0;
    int fragment_num = 1;
    
    while (offset < payload_length) {
        // Calculate current fragment size
        int current_data_size = (offset + max_data_size > payload_length) ? 
                               (payload_length - offset) : max_data_size;
        
        // Prepare IPv4 header
        IPv4Header header;
        init_ipv4_header(&header);
        header.identification = packet_id;
        header.mf = (offset + current_data_size < payload_length) ? 1 : 0; 
        header.fragment_offset = offset / FRAGMENT_UNIT;  
        header.total_length = IP_HEADER_SIZE + current_data_size;
        
        int path[MAX_NODES];
        for (int i = 0; i < MAX_NODES; i++) {
            path[i] = -1;
        }
        
        int path_length = findShortestPath(source, destination, topology, num_nodes, path);
        
        if (path_length < 0) {
            fprintf(stderr, "Error: No path found from %d to %d\n", source, destination);
            return;
        }
        
        char fragment_data[21];
        int copy_size = current_data_size > 20 ? 20 : current_data_size;
        strncpy(fragment_data, payload + offset, copy_size);
        fragment_data[copy_size] = '\0';
        
        printf("\033[1mFragment #%d:\033[0m\n", fragment_num);
        printf("  Header Size: %d bytes\n", IP_HEADER_SIZE);
        printf("  Data Size: %d bytes\n", current_data_size);
        printf("  Total Size: %d bytes\n", header.total_length);
        printf("  Offset: %d bytes (%d units)\n", offset, header.fragment_offset);
        printf("  Flags: [%s%s]\n", 
               header.df ? "DF " : "", 
               header.mf ? "MF" : "");
        printf("  Data: \"%s%s\"\n", fragment_data, current_data_size > 20 ? "..." : "");
        
        print_path_details(path, num_nodes, path_length);
        printf("\n");
        
        offset += current_data_size;
        fragment_num++;
    }
    
    printf("\033[1m==== Fragmentation Complete ====\033[0m\n");
}
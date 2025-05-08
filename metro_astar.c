#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_STATIONS 14 // number of stations from question
#define INF 1000000.0f  // value to represent “no connection” (INF = no direct connection)
#define LINES_COUNT 4
#define TRANSFER_TIME 3.0f

typedef struct {
    int station;                // current station index (starts from 0)
    float g;                    // cost (minutes)
    float f;                    // g + heuristic
    int path[NUM_STATIONS];     // sequence of stations visited
    int path_len;               // length of the path
    int last_line;              // line ID (-1 for start station)
} Node;

// Table 1: straight‑line distances in km
float direct_dist[NUM_STATIONS][NUM_STATIONS] = {
    //  E1    E2    E3    E4    E5    E6    E7    E8    E9   E10   E11   E12   E13   E14
    {    0,  4.3,  9.0, 14.7, 17.2, 13.1, 11.8, 11.3,  8.2, 10.7,  8.4, 14.1, 18.5, 17.3},
    {  4.3,    0,  5.3, 10.3, 13.1, 12.7, 10.3,  6.9,  4.3,  7.4,  5.9, 11.3, 14.8, 12.9},
    {  9.0,  5.3,    0,  5.9,  8.5, 10.9,  7.7,  4.1,  6.5,  8.9,  9.4, 14.5, 13.9, 10.3},
    { 14.7, 10.3,  5.9,    0,  2.9, 15.0, 12.7,  4.0,  9.1,  9.7, 12.2, 14.7, 10.6,  6.0},
    { 17.2, 13.1,  8.5,  2.9,    0, 16.0, 12.3,  7.0, 12.0, 15.3, 14.8, 17.3, 12.7,  6.9},
    { 13.1, 12.7, 10.9, 15.0, 16.0,    0,  3.2, 15.1, 16.5, 18.5, 19.0, 24.3, 25.2, 21.1},
    { 11.8, 10.3,  7.7, 12.7, 12.3,  3.2,    0, 12.0, 13.3, 16.4, 16.0, 22.2, 22.6, 17.1},
    { 11.3,  6.9,  4.1,  4.0,  7.0, 15.1, 12.0,    0,  5.0,  5.6,  7.9, 12.4,  9.8,  6.4},
    {  8.2,  4.3,  6.5,  9.1, 12.0, 16.5, 13.3,  5.0,    0,  3.0,  3.4,  8.1, 10.9,  9.6},
    { 10.7,  7.4,  8.9,  9.7, 15.3, 18.5, 16.4,  5.6,  3.0,    0,  3.4,  5.6,  7.7,  8.4},
    {  8.4,  5.9,  9.4, 12.2, 14.8, 19.0, 16.0,  7.9,  3.4,  3.4,    0,  5.9, 11.2, 12.7},
    { 14.1, 11.3, 14.5, 14.7, 17.3, 24.3, 22.2, 12.4,  8.1,  5.6,  5.9,    0,  8.6, 12.3},
    { 18.5, 14.8, 13.9, 10.6, 12.7, 25.2, 22.6,  9.8, 10.9,  7.7, 11.2,  8.6,    0,  6.1},
    { 17.3, 12.9, 10.3,  6.0,  6.9, 21.1, 17.1,  6.4,  9.6,  8.4, 12.7, 12.3,  6.1,    0}
};

// Table 2: actual distances in km
float real_dist[NUM_STATIONS][NUM_STATIONS] = {
    //  E1    E2    E3    E4    E5    E6    E7    E8    E9   E10   E11   E12   E13   E14
    {    0,  4.3,  INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF},
    {  4.3,    0,  5.3,  INF, INF,  INF,  14.3,  INF,  4.3,  INF,  INF,  INF,  INF,  INF},
    {  INF,  5.3,    0,  5.9,  INF,  INF,  8.5,  4.1,  INF,  INF,  INF,  INF,  INF,  INF},
    {  INF,  INF,  5.9,    0,  2.9,  INF,  INF,  4.0,  INF,  INF,  INF,  INF,  INF,  6.2},
    {  INF,  INF,  INF,  2.9,    0,  INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF},
    {  INF,  INF,  INF,  INF,  INF,    0,  3.2,  INF,  INF,  INF,  INF,  INF,  INF,  INF},
    {  INF, 14.3,  8.5,  INF,  INF,  3.2,    0,  INF,  INF,  INF,  INF,  INF,  INF,  INF},
    {  INF,  INF,  4.1,  4.0,  INF,  INF,  INF,    0,  5.0,  6.0,  INF,  INF,  INF,  INF},
    {  INF,  4.3,  INF,  INF,  INF,  INF,  INF,  5.0,    0,  3.0,  3.4,  INF,  INF,  INF},
    {  INF,  INF,  INF,  INF,  INF,  INF,  INF,  6.0,  3.0,    0,  INF,  5.6,  9.1,  INF},
    {  INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,  3.4,  INF,    0,  INF,  INF,  INF},
    {  INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,  5.6,  INF,    0,  INF,  INF},
    {  INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,  9.1,  INF,  INF,    0,  INF},
    {  INF,  INF,  INF,  6.2,  INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,    0}
};

// line 0: Red, line 1: Green, line 2: Blue, line 3: Yellow
int lines[LINES_COUNT][NUM_STATIONS] = {
    // E1  E2  E3  E4  E5  E6  E7  E8  E9  E10 E11 E12 E13 E14
    {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // Red
    {0, 1, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0}, // Green
    {0, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0}, // Blue
    {0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0}  // Yellow
};
int visited[NUM_STATIONS];

// heuristic: estimated travel time (minutes) at 40 km/h
float heuristic(int current, int goal) {
    if (direct_dist[current][goal] == INF)
        return INF;
    return (direct_dist[current][goal] / 40.0f) * 60.0f;
}

// print the found path as E1, E2 ... (0 in input is E1)
void print_path(int path[], int len) {
    printf("Path: ");
    for (int i = 0; i < len; i++) {
        printf("E%d ", path[i] + 1);
    }
    printf("\n");
}

//check if 2 stations share the same line
int select_line(int current_station, int next_station, int last_line)
{
    if (last_line>= 0 && lines[last_line][current_station] && lines[last_line][next_station]){
        return last_line;
    }
    for (int line = 0; line < LINES_COUNT; line++){
        if (lines[line][current_station] && lines[line][next_station]){
            return line;
        }
    }
    return -1;
}

// A* search from start to goal
void a_star(int start, int goal) {
    memset(visited, 0, sizeof(visited));
    Node frontier[100];
    int frontier_size = 0;

    // initialize start node
    Node initial = {0};
    initial.station   = start;
    initial.g         = 0.0f;
    initial.f         = heuristic(start, goal);
    initial.path[0]   = start;
    initial.path_len  = 1;
    initial.last_line = -1; // initial station
    frontier[frontier_size++] = initial;

    while (frontier_size > 0) {
        // pick node with lowest f
        int best_idx = 0;
        for (int i = 1; i < frontier_size; i++) {
            if (frontier[i].f < frontier[best_idx].f)
                best_idx = i;
        }
        Node current = frontier[best_idx];

        // goal test before removal
        if (current.station == goal) {
            printf("Found! Total cost: %.2f minutes\n", current.g);
            print_path(current.path, current.path_len);
            return;
        }

        // remove node from frontier
        for (int i = best_idx; i < frontier_size - 1; i++)
            frontier[i] = frontier[i + 1];
        frontier_size--;
        
        //mark as visited
        visited[current.station] = 1;

        // expand neighbors
        for (int next_station = 0; next_station < NUM_STATIONS; next_station++) {
            if (real_dist[current.station][next_station] != INF && !visited[next_station]) {
                // travel time to neighbor
                float travel_time = (real_dist[current.station][next_station] / 40.0f) * 60.0f;

                int edge_line = select_line(current.station, next_station, current.last_line);
                // check changing lines (penalty 3 min)
                if (current.last_line != -1 && edge_line != current.last_line){
                    travel_time += TRANSFER_TIME;
                }

                Node neighbor = {0};
                neighbor.station  = next_station;
                neighbor.g        = current.g + travel_time;
                neighbor.f        = neighbor.g + heuristic(next_station, goal);

                // copy current path into neighbor
                memcpy(neighbor.path, current.path, sizeof(int) * current.path_len);

                // every successor start with the same route (current)
                // we just need to add a new frontier
                neighbor.path[current.path_len] = next_station;
                neighbor.path_len = current.path_len + 1;
                frontier[frontier_size++] = neighbor;
            }
        }

        // log current frontier
        printf("Frontier: ");
        for (int i = 0; i < frontier_size; i++) {
            printf("[E%d f=%.2f] ",
                   frontier[i].station + 1,
                   frontier[i].f);
        }
        printf("\n");
    }

    printf("No path found.\n");
}

int main(int argc, char* argv[]) {
    // default: from E1 (0) to E14 (13)
    int start = 0;
    int goal  = 13;
    if (argc >= 3) {
        start = atoi(argv[1]);
        goal  = atoi(argv[2]);
    }

    if (start < 0 || start >= NUM_STATIONS ||
        goal  < 0 || goal  >= NUM_STATIONS) {
        fprintf(stderr, "Usage: %s [start(0-%d)] [goal(0-%d)]\n",
                argv[0], NUM_STATIONS-1, NUM_STATIONS-1);
        return 1;
    }

    a_star(start, goal);
    return 0;
}

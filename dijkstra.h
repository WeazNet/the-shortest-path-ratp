#include "tab_hachage.h"

#define COST_PER_LINE 5
#define COST_PER_STATION 1
#define CODE_END -99

int32_t get_minimum_cost_shortest_path(liste *file);
void dijkstra(tab_hachage *th, uint32_t source);
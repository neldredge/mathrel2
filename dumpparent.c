#include <stdio.h>
#include "graph.h"

int main(void) {
  const_graph G = map_graph(0);
  if (!G)
    return 1;
  vertex_t v, c;
  for (v = 0; v < G_TOTAL_VERTICES(G); v++) {
    for (c = 0; c < G_NUM_PARENTS(G,v); c++) {
      printf("%u %u\n",  (unsigned)G_PARENT(G,v,c), (unsigned)v);
    }
  }
  return 0;
}


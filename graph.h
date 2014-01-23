#ifndef GRAPH_H
#define GRAPH_H

#include <stdint.h>

// A directed graph G is represented by an array of uint32_t.
// Vertices are numbered 1..n.
// G[0] : number of vertices
// G[1..n] : indices of edge lists.  Terminates with 0.
// G[G[i]] : list of children of vertex i.  G[G[i]] is the first child.  
// The list terminates with 0.


#endif // GRAPH_H

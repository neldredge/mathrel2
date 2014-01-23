#ifndef GRAPH_H
#define GRAPH_H

#include <stdint.h>

/* 
   Should have:

   - Magic number
   - Version number
   - Total size of header
   - Total size of structure
   - Number of vertices
   - Number of edges
   - For each edge, list of parents
   - For each edge, list of children
   - 

*/



/* 
 * Graph data structure, suitable for mmap.
 * An array G of uint32_t.
 * G[0]: magic number GRAPH_MAGIC
 * G[1]: version number
 * G[2]: Total size of header
 * G[3]: Total size of structure
 * G[4]: Number of vertices
 * G[5]: Number of edges
 * G[6]: Size of each vertex directory entry.
 * G[7....]: Vertex directory.  For each vertex, four values:
 * - Number of parents
 * - Index to list of parents
 * - Number of children
 * - Index to list of children
 * G[...]: Following vertex list, edge data (lists of parents and children)
 */

typedef uint32_t vertex_t;
typedef vertex_t *graph;
typedef const vertex_t *const_graph;

/* The G argument of these macros must not have side effects. */
#define GRAPH_MAGIC_NUMBER 987654321U
#define GRAPH_VERSION 1 /* This is version 1 */

#define G_MAGIC(G)            ((G)[0])
#define G_VERSION(G)          ((G)[1])
#define G_HEADER_SIZE(G)      ((G)[2])
#define G_STRUCTURE_SIZE(G)   ((G)[3])
#define G_TOTAL_VERTICES(G)   ((G)[4])
#define G_TOTAL_EDGES(G)      ((G)[5]) 
#define G_ENTRY_SIZE(G)       ((G)[6])
/* Unused words */
#define GRAPH_HEADER_SIZE 8

#define G_VERTEX_ENTRY_START(G,v)   (G_HEADER_SIZE(G) + ((v) * G_ENTRY_SIZE(G)))
#define G_NUM_PARENTS(G,v)          ((G)[G_VERTEX_ENTRY_START((G),(v)) + 0])
#define G_PARENT_LIST_START(G,v)    ((G)[G_VERTEX_ENTRY_START((G),(v)) + 1])
#define G_NUM_CHILDREN(G,v)         ((G)[G_VERTEX_ENTRY_START((G),(v)) + 2])
#define G_CHILD_LIST_START(G,v)     ((G)[G_VERTEX_ENTRY_START((G),(v)) + 3])
#define GRAPH_VERTEX_ENTRY_SIZE 4 /* number of words in each entry in the vertex directory */


#define G_EDGE(G,e)                 ((G)[G_VERTEX_ENTRY_START((G), G_TOTAL_VERTICES(G)) + (e)])

#define G_PARENT(G,v,i)             G_EDGE((G), G_PARENT_LIST_START((G), (v)) + (i))
#define G_CHILD(G,v,i)              G_EDGE((G), G_CHILD_LIST_START((G), (v)) + (i))

#define GRAPH_NEEDED_STRUCTURE_SIZE(v,e) (GRAPH_HEADER_SIZE + ((v) * GRAPH_VERTEX_ENTRY_SIZE) + (2 * (e)))

#endif /* GRAPH_H */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "graph.h"

/* Usage: populate foo.g < edges */
/* Takes a list of edges on standard input; dumps the graph to foo.g */

struct edge {
  vertex_t p, c;
};

struct edge *edges;
size_t num_edges, edges_alloced;

vertex_t max_vertex;
vertex_t num_vertices;
vertex_t *parent_count, *child_count;

void count_neighbors(void);

void populate_edges(FILE *in) {
  unsigned int p, c;
  while (fscanf(in, "%u %u\n", &p, &c) == 2) {
    assert(num_edges <= edges_alloced);
    if (num_edges == edges_alloced) {
      /* Need more memory */
      edges_alloced++; /* in case it was 0 */
      edges_alloced *= 2;
      edges = realloc(edges, edges_alloced * sizeof(struct edge));
      if (!edges) {
	perror("realloc of edges");
	exit(2);
      }
    }
    assert(num_edges < edges_alloced);
    if (p > max_vertex)
      max_vertex = p;
    if (c > max_vertex)
      max_vertex = c;
    edges[num_edges].p = p;
    edges[num_edges].c = c;
    num_edges++;
  }
  if (ferror(in)) {
    perror("reading input");
    exit(1);
  }
  num_vertices = max_vertex + 1;
  count_neighbors();
}

void count_neighbors(void) {
  struct edge *e;
  parent_count = calloc(num_vertices, sizeof(vertex_t));
  child_count = calloc(num_vertices, sizeof(vertex_t));
  if (!parent_count || !child_count) {
    perror("calloc of counts");
    exit(2);
  }
  for (e = edges; e < edges + num_edges; e++) {
    parent_count[e->c]++;
    child_count[e->p]++;
  }
}

graph populate_graph(void) {
  graph G;
  size_t s = GRAPH_NEEDED_STRUCTURE_SIZE(num_vertices, num_edges);
  G = calloc(s, sizeof(vertex_t));
  if (!G) {
    perror("calloc of graph");
    exit(2);
  }

  G_MAGIC(G) = GRAPH_MAGIC_NUMBER;
  G_VERSION(G) = GRAPH_VERSION;
  G_HEADER_SIZE(G) = GRAPH_HEADER_SIZE;
  G_STRUCTURE_SIZE(G) = s;
  G_TOTAL_VERTICES(G) = num_vertices;
  G_TOTAL_EDGES(G) = num_edges; /* every edge appears twice */
  G_ENTRY_SIZE(G) = GRAPH_VERTEX_ENTRY_SIZE;

  /* Allocate space for edge lists */
  vertex_t v;
  vertex_t i = 0;
  for (v = 0; v < num_vertices; v++) {
    G_NUM_PARENTS(G,v) = 0; /* will be filled in later */
    G_PARENT_LIST_START(G,v) = i;
    i += parent_count[v];

    G_NUM_CHILDREN(G,v) = 0;
    G_CHILD_LIST_START(G,v) = i;
    i += child_count[v];
    assert(&G_EDGE(G,i) <= G+s);
  }

  /* Fill in edge lists */
  struct edge *e;
  for (e = edges; e < edges + num_edges; e++) {
    G_PARENT(G, e->c, G_NUM_PARENTS(G, e->c)++) = e->p;
    G_CHILD(G, e->p, G_NUM_CHILDREN(G, e->p)++) = e->c;
  }
  
  /* Re-validate */
  for (v = 0; v < num_vertices; v++) {
    assert(G_NUM_PARENTS(G, v) == parent_count[v]);
    assert(&G_PARENT(G, v, G_NUM_PARENTS(G,v)) <= G+s);
    assert(G_NUM_CHILDREN(G,v) == child_count[v]);
    assert(&G_CHILD(G, v, G_NUM_CHILDREN(G,v)) <= G+s);
  }

  return G;
}

void write_graph(const_graph G, const char *filename) {
  int fd;
  if ((fd = open(filename, O_CREAT | O_WRONLY, 0644)) < 0) {
    perror(filename);
    exit(1);
  }
  if (flock(fd, LOCK_EX) < 0) {
    perror("flock");
    exit(1);
  }
  if (ftruncate(fd, 0) < 0) {
    perror("ftruncate");
    exit(1);
  }
  
  /* Write everything out */
  const char *p = (const char *)G;
  size_t s = G_STRUCTURE_SIZE(G) * sizeof(vertex_t);
  const char *end = p + s;
  while (p < end) {
    errno = 0;
    ssize_t res = write(fd, p, end - p);
    if (res > 0) {
      p += res;
    } else if (errno != EINTR) {
      perror("write");
      exit(1);
    }
  }
  
  if (flock(fd, LOCK_UN) < 0) {
    perror("flock(LOCK_UN)");
  }
  
  if (close(fd) < 0) {
    perror("close");
  }

}


int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s foo.g < edgelist\n", argv[0]);
    exit(3);
  }
  populate_edges(stdin);
  write_graph(populate_graph(), argv[1]);
  return 0;
}

  
    

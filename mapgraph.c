#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <stdlib.h>
#include <stdio.h>
#include "graph.h"

int check_sanity(const_graph G, off_t sz);

/* Not great error handling here */
const_graph map_graph(int fd) {
  if (flock(fd, LOCK_SH) < 0) {
    perror("flock(LOCK_SH)");
    return NULL;
  }
  struct stat s;
  if (fstat(fd, &s) < 0) {
    perror("fstat");
    goto unlock;
  }
  off_t sz = s.st_size;
  void *p;
  if ((p = mmap(NULL, sz, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0)) == MAP_FAILED) {
    perror("mmap");
    goto unlock;
  }

  const_graph G = (const_graph)p;
  if (check_sanity(G, sz) == 0)
    return G;

 unmap:
  if (munmap(p, sz) < 0)
    perror("munmap");
 unlock:
  if (flock(fd, LOCK_UN) < 0)
    perror("flock(LOCK_UN)");

  return NULL;
}

int check_sanity(const_graph G, off_t sz) {
  if (G_MAGIC(G) != GRAPH_MAGIC_NUMBER) {
    fprintf(stderr, "Bad magic\n");
    return -1;
  }
  if (G_VERSION(G) != GRAPH_VERSION) {
    fprintf(stderr, "Graph version mismatch\n");
    return -2;
  }
  if (sz < GRAPH_HEADER_SIZE) {
    fprintf(stderr, "Graph smaller than header\n");
    return -3;
  }
  if (sz != G_STRUCTURE_SIZE(G) * sizeof(vertex_t)) {
    fprintf(stderr, "Wrong structure size\n");
    return -4;
  }
  if (G_STRUCTURE_SIZE(G) != GRAPH_NEEDED_STRUCTURE_SIZE(G_TOTAL_VERTICES(G), G_TOTAL_EDGES(G))) {
    fprintf(stderr, "Wrong size for number of vertices/edges\n");
    return -5;
  }
  return 0;
}

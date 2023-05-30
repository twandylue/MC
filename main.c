#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#define HEAP_CAPACITY 640000
#define HEAP_ALLOCATED_CAPACITY 1024

char heap[HEAP_CAPACITY] = {0};
size_t heap_size = 0;

typedef struct {
  void *start;
  size_t size;
} Heap_Chunk;

Heap_Chunk heap_allocated[HEAP_ALLOCATED_CAPACITY] = {0};
size_t heap_allocated_size = 0;

void *heap_alloc(size_t size) {
  assert(heap_size + size <= HEAP_CAPACITY);
  void *result = heap + heap_size;
  heap_size += size;

  const Heap_Chunk chunk = {.start = result, .size = size};
  assert(heap_allocated_size < HEAP_ALLOCATED_CAPACITY);
  heap_allocated[heap_allocated_size++] = chunk;

  return result;
}

void heap_free(void *ptr) {
  (void)ptr;
  assert(false && "TODO: heap_free is not implemented yet");
}

void heap_collect() {
  assert(false && "TODO: heap_collect is not implemented yet");
}

void heap_dump_allcated_chunk(void) {
  printf("Allocated Chunks: (%zu)\n", heap_allocated_size);
  for (size_t i = 0; i < heap_allocated_size; ++i) {
    printf(" start: %p, size: %zu\n", heap_allocated[i].start,
           heap_allocated[i].size);
  }
}

int main(void) {
  // for (int i = 0; i < 100; ++i) {
  //   heap_alloc(i);
  // }
  //
  // heap_dump_allcated_chunk();

  char *root = heap_alloc(26);
  for (int i = 0; i < 26; ++i) {
    root[i] = i + 'A';
  };

  heap_dump_allcated_chunk();

  return 0;
}

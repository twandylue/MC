#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define HEAP_CAPACITY 640000
#define CHUNK_LIST_CAPACITY 1024

char heap[HEAP_CAPACITY] = {0};
size_t heap_size = 0;

typedef struct {
  char *start;
  size_t size;
} Chunk;

typedef struct {
  size_t count;
  Chunk chunks[CHUNK_LIST_CAPACITY];
} Chunk_List;

Chunk heap_allocated[CHUNK_LIST_CAPACITY] = {0};
size_t heap_allocated_size = 0;

Chunk_List allocated_chunks = {0};
Chunk_List freed_chunks = {0};

// NOTE: macro for todo
#define UNIMPLEMENTED                                                          \
  do {                                                                         \
    fprintf(stderr, "UNIMPLEMENTED:\n");                                       \
    fprintf(stderr, "In func: %s\n", __func__);                                \
    fprintf(stderr, "In file: %s\n", __FILE__);                                \
    fprintf(stderr, "In line: %d\n", __LINE__);                                \
    abort();                                                                   \
  } while (0)

int chunk_start_compare(const void *a, const void *b) {
  const Chunk *a_chunk = a;
  const Chunk *b_chunk = b;

  return a_chunk->start - b_chunk->start;
}

int chunk_list_find(const Chunk_List *list, void *ptr) {
  Chunk key = {.start = ptr};
  Chunk *result = bsearch(&key, list->chunks, list->count, sizeof(Chunk),
                          chunk_start_compare);
  if (result != NULL) {
    assert(list->chunks <= result);

    // BUG:
    // long placeholder = (result - list->chunks);
    // printf("placeholder: %ld\n", placeholder);
    // printf("sizeof: %ld\n", sizeof(Chunk));
    // return placeholder / sizeof(Chunk);
    return (result - list->chunks);
  } else {
    return -1;
  }
}

void chunk_list_insert(Chunk_List *list, void *start, size_t size) {
  assert(list->count < CHUNK_LIST_CAPACITY);
  list->chunks[list->count].start = start;
  list->chunks[list->count].size = size;

  for (size_t i = list->count;
       i > 0 && list->chunks[i].start < list->chunks[i - 1].start; --i) {
    const Chunk t = list->chunks[i];
    list->chunks[i] = list->chunks[i - 1];
    list->chunks[i - 1] = t;
  }

  list->count += 1;
}

void chunk_list_remove(Chunk_List *list, size_t index) {
  assert(index < list->count);
  for (size_t i = index; i < list->count - 1; --i) {
    list->chunks[i] = list->chunks[i + 1];
  }

  list->count -= 1;
}

void chunk_list_dump(const Chunk_List *list) {
  printf("Allocated Chunks: (%zu)\n", list->count);

  for (size_t i = 0; i < list->count; ++i) {
    printf(" start: %p, size: %zu\n", list->chunks[i].start,
           list->chunks[i].size);
  }
}

void *heap_alloc(size_t size) {
  if (size > 0) {

    assert(heap_size + size <= HEAP_CAPACITY);
    void *ptr = heap + heap_size;
    heap_size += size;

    chunk_list_insert(&allocated_chunks, ptr, size);

    return ptr;
  } else {
    return NULL;
  }
}

void heap_free(void *ptr) {
  if (ptr != NULL) {
    const int index = chunk_list_find(&allocated_chunks, ptr);
    printf("index: %d\n", index);
    assert(index >= 0);

    chunk_list_insert(&freed_chunks, allocated_chunks.chunks[index].start,
                      allocated_chunks.chunks[index].size);
    chunk_list_remove(&allocated_chunks, (size_t)index);
  }
}

void heap_collect() { UNIMPLEMENTED; }

int main(void) {
  for (int i = 0; i < 10; ++i) {
    void *p = heap_alloc(i);
    if (i % 2 == 0) {
      heap_free(p);
    }
  }

  chunk_list_dump(&allocated_chunks);
  chunk_list_dump(&freed_chunks);

  return 0;
}
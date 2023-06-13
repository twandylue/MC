#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define HEAP_CAPACITY 640000
#define CHUNK_LIST_CAPACITY 1024

char heap[HEAP_CAPACITY] = {0};

typedef struct {
  char *start;
  size_t size;
} Chunk;

typedef struct {
  size_t count;
  Chunk chunks[CHUNK_LIST_CAPACITY];
} Chunk_List;

Chunk_List allocated_chunks = {0};
Chunk_List freed_chunks = {.count = 1,
                           .chunks = {
                               [0] = {.start = heap, .size = sizeof(heap)},
                           }};
Chunk_List temp_chunks = {0};

// NOTE: macro for todo
#define UNIMPLEMENTED                                                          \
  do {                                                                         \
    fprintf(stderr, "UNIMPLEMENTED:\n");                                       \
    fprintf(stderr, "In func: %s\n", __func__);                                \
    fprintf(stderr, "In file: %s\n", __FILE__);                                \
    fprintf(stderr, "In line: %d\n", __LINE__);                                \
    abort();                                                                   \
  } while (0)

void chunk_list_insert(Chunk_List *list, void *start, size_t size) {
  assert(list->count < CHUNK_LIST_CAPACITY);

  list->chunks[list->count].start = start;
  list->chunks[list->count].size = size;

  // NOTE: ascending sorting
  for (size_t i = list->count;
       i > 0 && list->chunks[i].start < list->chunks[i - 1].start; --i) {
    const Chunk t = list->chunks[i];
    list->chunks[i] = list->chunks[i - 1];
    list->chunks[i - 1] = t;
  }

  list->count += 1;
}

void chunk_list_merge(Chunk_List *dst, const Chunk_List *src) {
  dst->count = 0;
  for (size_t i = 0; i < src->count; ++i) {
    const Chunk chunk = src->chunks[i];

    if (dst->count > 0) {
      Chunk *top_chunk = &dst->chunks[dst->count - 1];

      if (top_chunk->start + top_chunk->size == chunk.start) {
        top_chunk->size += chunk.size;
      } else {
        chunk_list_insert(dst, chunk.start, chunk.size);
      }
    } else {
      chunk_list_insert(dst, chunk.start, chunk.size);
    }
  }
}

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

    return (result - list->chunks);
  } else {
    return -1;
  }
}

// NOTE: Search in the linear way
/* int chunk_list_find(const Chunk_List *list, void *ptr) { */
/*   for (size_t i = 0; i < list->count; ++i) { */
/*     if (list->chunks[i].start == ptr) { */
/*       return (int)i; */
/*     } */
/*   } */
/*   return -1; */
/* } */

void chunk_list_remove(Chunk_List *list, size_t index) {
  assert(index < list->count);

  for (size_t i = index; i < list->count - 1; ++i) {
    list->chunks[i] = list->chunks[i + 1];
  }

  list->count -= 1;
}

void chunk_list_dump(const Chunk_List *list) {
  printf("Chunks: (%zu)\n", list->count);

  for (size_t i = 0; i < list->count; ++i) {
    printf("  start: %p, size: %zu\n", (void *)list->chunks[i].start,
           list->chunks[i].size);
  }
}

void *heap_alloc(size_t size) {
  chunk_list_merge(&temp_chunks, &freed_chunks);
  freed_chunks = temp_chunks;

  if (size > 0) {
    // NOTE: 'chunks' is an ascending array storing address.
    for (size_t i = 0; i < freed_chunks.count; ++i) {
      const Chunk chunk = freed_chunks.chunks[i];
      if (chunk.size >= size) {
        chunk_list_remove(&freed_chunks, i);

        const size_t tail_size = chunk.size - size;
        chunk_list_insert(&allocated_chunks, chunk.start, size);

        if (tail_size > 0) {
          chunk_list_insert(&freed_chunks, chunk.start + size, tail_size);
        }

        return chunk.start;
      }
    }
  }

  return NULL;
}

void heap_free(void *ptr) {
  if (ptr != NULL) {
    const int index = chunk_list_find(&allocated_chunks, ptr);
    assert(index >= 0);
    assert(ptr == allocated_chunks.chunks[index].start);

    chunk_list_insert(&freed_chunks, allocated_chunks.chunks[index].start,
                      allocated_chunks.chunks[index].size);
    chunk_list_remove(&allocated_chunks, (size_t)index);
  }
}

void heap_collect() { UNIMPLEMENTED; }

#define N 10

void *ptrs[N] = {0};

int main(void) {
  for (int i = 0; i < N; ++i) {
    ptrs[i] = heap_alloc(i);
  }

  // NOTE: Continuous freed chunks and those would be merged into a single
  // chunk.
  /* for (int i = 0; i < N; ++i) { */
  /*   heap_free(ptrs[i]); */
  /* } */

  // NOTE: Fragmented chunks and those would not be merged into a single chunk.
  // Thus, we need a GC.
  for (int i = 0; i < N; ++i) {
    if (i % 2 == 0) {
      heap_free(ptrs[i]);
    }
  }

  heap_alloc(10);

  printf("* allocated chunks:\n");
  chunk_list_dump(&allocated_chunks);

  printf("* free chunks:\n");
  chunk_list_dump(&freed_chunks);

  return 0;
}

struct block_meta {
  size_t size;
  struct block_meta *next;
  int free;
  uint32_t magic; // For debugging only. TODO: remove this in non-debug mode.
};


void *_z80malloc(size_t size);
void *_z80realloc(void *ptr, size_t size);
void _z80free(void *ptr);
void *_z80calloc(size_t nmemb, size_t size);
void *_z80realloc(void *ptr, size_t size);
void __sdcc_heap_init();



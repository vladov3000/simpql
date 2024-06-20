struct Arena {
  U8* memory    = nullptr;
  U64 used      = 0;
  U64 committed = 0;
  U64 size      = 0;
};

static Arena make_arena(U64 size) {
  U8* memory = (U8*) mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (memory == MAP_FAILED) {
    print(ERROR "Failed to reserve memory: ", strerror(errno), ".\n");
    exit(EXIT_FAILURE);
  }

  Arena arena;
  arena.memory = memory;
  arena.size   = size;
  return arena;
}

static U64 align(U64 offset, U64 alignment) {
  return (offset + alignment - 1) & ~(alignment - 1);
}

static U8* allocate_bytes(Arena* arena, U64 size, U64 alignment) {
  U8* memory    = arena->memory;
  U64 used      = align(arena->used, alignment);
  U64 committed = arena->committed;
  
  U8* result  = &memory[used];
  used       += size;
  arena->used = used;

  if (used > committed) {
    U64 difference = align(used - committed, 4096);
    if (committed > arena->size - difference) {
      print("Arena out of memory.\n");
      exit(EXIT_FAILURE);
    }
    
    if (mprotect(&memory[committed], difference, PROT_READ | PROT_WRITE) == -1) {
      print(WARN "Failed to commit memory.\n");
    }
    
    arena->committed += difference;
  }
  return result;
}

static U8* allocate_bytes(Arena* arena, U64 size) {
  return allocate_bytes(arena, size, 1);
}

template <typename T>
static T* allocate(Arena* arena) {
  return (T*) allocate_bytes(arena, sizeof(T), alignof(T));
}

template <typename T>
static Span<T> allocate_span(Arena* arena, I64 size) {
  T* data = (T*) allocate_bytes(arena, sizeof(T) * size, alignof(T));
  return Span(data, size);
}

static String allocate_string(Arena* arena, I64 size) {
  U8* data = (U8*) allocate_bytes(arena, size, 1);
  return String(data, size);
}

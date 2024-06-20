struct String {
  U8* data = nullptr;
  I64 size = 0;

  String() {
  }

  String(const char* s) {
    data = (U8*) s;
    size = s == NULL ? 0 : strlen(s);
  }

  String(U8* data, I64 size) {
    this->data = data;
    this->size = size;
  }

  U8* begin() {
    return data;
  }

  U8* end() {
    return &data[size];
  }

  U8& operator[](I64 index) {
    return data[index];
  }

  operator bool() {
    return size > 0;
  }
};

static bool operator==(String a, String b) {
  return a.size == b.size && memcmp(a.data, b.data, a.size) == 0;
}

static String slice(String base, I64 start) {
  return String(&base[start], max(base.size - start, 0l));
}

static String slice(String base, I64 start, I64 count) {
  return String(&base[start], min(base.size - start, count));
}

static B32 is_letter(U8 c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

static B32 is_space(U8 c) {
  return c == ' ' || c == '\t' || c == '\n';
}

static B32 is_digit(U8 c) {
  return '0' <= c && c <= '9';
}

struct Arena;

static U8* allocate_bytes(Arena* arena, U64 size);
static String allocate_string(Arena* arena, I64 size);

static String copy(Arena* arena, String input) {
  String output = allocate_string(arena, input.size);
  memcpy(output.data, input.data, input.size);
  return output;
}

static char* concat(Arena* arena, const char* a, const char* b) {
  U64   a_size = strlen(a);
  U64   b_size = strlen(b);
  char* result = (char*) allocate_bytes(arena, a_size + b_size + 1);
  strcpy(result, a);
  strcpy(&result[a_size], b);
  return result;
}

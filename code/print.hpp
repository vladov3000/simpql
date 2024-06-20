#define INFO  "\x1b[32;1m[ Info  ]\x1b[0m "
#define WARN  "\x1b[33;1m[ Warn  ]\x1b[0m "
#define ERROR "\x1b[31;1m[ Error ]\x1b[0m "


static U8  print_buffer[4096];
static I64 print_buffered;

static void flush() {
  if (print_buffered > 0) {
    write(STDOUT_FILENO, print_buffer, print_buffered);
    print_buffered = 0;
  }
}

static void print(String message) {
  if (message.size > sizeof print_buffer) {
    flush();
    write(STDOUT_FILENO, message.data, message.size);
  } else {
    if (message.size > sizeof print_buffer - print_buffered) {
      flush();
    }
    memcpy(&print_buffer[print_buffered], message.data, message.size);
    print_buffered += message.size;
  }
}

static void print(U8 c) {
  if (print_buffered == sizeof print_buffer) {
    flush();
  }
  print_buffer[print_buffered] = c;
  print_buffered++;
}

static void print(auto&& first, auto&& second, auto&&... rest) {
  print(first);
  print(second);
  (print(rest), ...);
}

static void btree_insert(Arena* arena, I32 fd, I64 key, I64 value) {
  String buffer     = allocate_string(arena, 4096);
  I32    bytes_read = read(buffer, buffer.data, buffer.size);
  if (bytes_read == -1) {
    print(ERROR "Failed to read: ", strerror(errno), ".\n");
    exit(EXIT_FAILURE);
  }

  String input = slice(buffer, 0, bytes_read);
  
}

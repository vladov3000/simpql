static void make_folder(char* path) {
  if (mkdir(path, 0777) == -1 && errno != EEXIST) {
    print(ERROR, "Failed to make folder at \"", path, "\": ", strerror(errno), ".\n");
    exit(EXIT_FAILURE);
  }
}

static void execute(Arena* arena, Table create_table, char* database) {
  make_folder(database);
  make_folder(concat(arena, database, "/tables"));
}

static void execute(Arena* arena, Statement statement, char* database) {
  switch (statement.kind) {

  case Statement::EMPTY:
    break;

  case Statement::CREATE_TABLE:
    execute(arena, statement.create_table, database);
    break;
    
  }
}

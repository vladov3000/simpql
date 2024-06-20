enum Type {
  TYPE_INTEGER,
  TYPE_TEXT,
  TYPE_BLOB,
};

#define PRIMARY_KEY (1 << 0)

struct Column {
  String name;
  Type   type;
  U32    flags;
};

struct Table {
  String       name;
  Span<Column> columns;
};

struct Statement {
  enum Kind {
    EMPTY,
    CREATE_TABLE,
  };

  Kind kind;
  union {
    Table create_table;
  };
};

static void print(Type type) {
  switch (type) {
  case TYPE_INTEGER: print("INTEGER"); break;
  case TYPE_TEXT:    print("TEXT");    break;
  case TYPE_BLOB:    print("BLOB");    break;
  };
}

static void print(Column column) {
  print(column.name, ' ', column.type);
  if (column.flags & PRIMARY_KEY) {
    print(" PRIMARY KEY");
  }
}

static void print(Table create_table) {
  print("CREATE TABLE ", create_table.name, "(");
  for (I64 i = 0; i < create_table.columns.size; i++) {
    if (i > 0) {
      print(", ");
    }
    print(create_table.columns[i]);
  }
  print(");");
}

static void print(Statement statement) {
  switch (statement.kind) {

  case Statement::EMPTY:
    print(";");
    break;
    
  case Statement::CREATE_TABLE:
    print(statement.create_table);
    break;
    
  };
}

static void validate(Table create_table) {
  B32 found_primary_key = false;
  for (I64 i = 0; i < create_table.columns.size; i++) {
    Column column = create_table.columns[i];
    for (I64 j = 0; j < create_table.columns.size; j++) {
      if (i != j && column.name == create_table.columns[j].name) {
	print(ERROR, create_table.name, " has duplicate column ", column.name, ".\n");
	exit(EXIT_FAILURE);	
      }
    }
    if (column.flags & PRIMARY_KEY) {
      if (found_primary_key) {
	print(ERROR, create_table.name, " has multiple primary keys.\n");
	exit(EXIT_FAILURE);
      }
      found_primary_key = true;
    }
  }
  if (!found_primary_key) {
    print(ERROR, create_table.name, " is missing a primary key.\n");
    exit(EXIT_FAILURE);
  }
}

static void validate(Statement statement) {
  switch (statement.kind) {

  case Statement::EMPTY:
    break;
    
  case Statement::CREATE_TABLE:
    validate(statement.create_table);
    break;
    
  };
}

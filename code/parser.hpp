struct Parser {
  Arena*    statement_arena;
  Arena*    column_arena;
  Statement statement;
  I32       counter;
};

static Parser* make_parser(Arena* statement_arena, Arena* column_arena) {
  Parser* parser          = allocate<Parser>(statement_arena);
  parser->statement_arena = statement_arena;
  parser->column_arena    = column_arena;
  parser->statement.kind  = Statement::EMPTY;
  parser->counter         = 0;
  return parser;
}

static void expected(String expected, String got) {
  print(ERROR "Expected \"", expected, "\", got \"", got, "\".\n");
  exit(EXIT_FAILURE);
}

static Span<Statement> parse(Parser* parser, Span<String> tokens) {
  Arena*          statement_arena = parser->statement_arena;
  Arena*          column_arena    = parser->column_arena;
  Statement*      statement       = &parser->statement;
  Span<Statement> statements      = allocate_span<Statement>(statement_arena, 0);
  
  for (String token : tokens) {
    switch (statement->kind) {
    
    case Statement::EMPTY:
      if (token == "CREATE") {
	statement->kind = Statement::CREATE_TABLE;
	parser->counter = 0;
      } else {
	expected("CREATE, SELECT, or INSERT", token);
      }
      break;

    case Statement::CREATE_TABLE: {
      Table* create_table = &statement->create_table;
      switch (parser->counter) {
	
      case 0:
	if (token != "TABLE") {
	  expected("TABLE", token);
	}
	parser->counter++;
	break;

      case 1:
	create_table->name = token;
	parser->counter++;
	break;

      case 2:
	if (token != "(") {
	  expected("(", token);
	}
	create_table->columns = allocate_span<Column>(column_arena, 0);
	parser->counter++;
	break;

      case 3: {
	if (token == ")") {
	  parser->counter = 7;
	} else {
	  Column* column = allocate<Column>(column_arena);
	  column->name   = token;
	  column->flags  = 0;
	  parser->counter++;
	}
	break;
      }

      case 4: {
	Column* column = create_table->columns.end();
	if (token == "INTEGER") {
	  column->type = TYPE_INTEGER;
	} else if (token == "TEXT") {
	  column->type = TYPE_TEXT;
	} else if (token == "BLOB") {
	  column->type = TYPE_BLOB;
	} else {
	  expected("INTEGER, TEXT, or BLOB", token);
	}
	parser->counter++;
	break;
      }

      case 5:
	if (token == "PRIMARY") {
	  Column* column = create_table->columns.end();
	  if (column->flags & PRIMARY_KEY) {
	    print(ERROR "Already specified ", column->name, "as a PRIMARY KEY.\n");
	    exit(EXIT_FAILURE);
	  }
	  parser->counter++;
	} else if (token == ",") {
	  create_table->columns.size++;
	  parser->counter = 3;
	} else if (token == ")") {
	  create_table->columns.size++;
	  parser->counter = 7;
	} else {
	  expected("PRIMARY KEY, comma or closing paranthesis", token);
	}
	break;

      case 6:
	if (token == "KEY") {
	  Column* column  = create_table->columns.end();
	  if (column->type != TYPE_INTEGER) {
	    print(ERROR "Only INTEGERs may be PRIMARY KEYs, but ",
		  column->name, " has type ", column->type, ".\n");
	    exit(EXIT_FAILURE);
	  }
	  column->flags  |= PRIMARY_KEY;
	  parser->counter = 5;
	} else {
	  expected("KEY", token);
	}
	break;

      case 7:
	if (token != ";") {
	  expected(";", token);
	}
	*allocate<Statement>(statement_arena) = *statement;
	statements.size++;
	statement->kind = Statement::EMPTY;
	parser->counter = 0;
	break;
	
      }
      break;
    }

    }
  }
  return statements;
}

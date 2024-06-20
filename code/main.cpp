#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include "basic.hpp"
#include "string.hpp"
#include "span.hpp"
#include "print.hpp"
#include "arena.hpp"
#include "syntax.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "execute.hpp"

I32 main(I32 argc, char** argv) {
  atexit(flush);

  if (argc != 3) {
    print(ERROR "Expected exactly 2 arguments.\n"
	  "Usage: simpql DATABASE INPUT_FILE\n");
    exit(EXIT_FAILURE);
  }

  char* database = argv[1];
  char* queries  = argv[2];
  
  I32 fd = open(queries, O_RDONLY);
  if (fd == -1) {
    print(ERROR "Failed to open ", queries, ": ", strerror(errno), ".\n");
    exit(EXIT_FAILURE);
  }

  Arena arenas[5];
  for (I64 i = 0; i < length(arenas); i++) {
    arenas[i] = make_arena(1ull << 32);
  }
  
  String  buffer = allocate_string(&arenas[0], 4096);
  Lexer*  lexer  = make_lexer(&arenas[0], &arenas[1]);
  Parser* parser = make_parser(&arenas[2], &arenas[3]);

  I64 bytes_read = 1;
  while (bytes_read > 0) {
    bytes_read = read(fd, buffer.data, buffer.size);
    if (bytes_read == -1) {
      print(ERROR "Failed to read input: ", strerror(errno), ".\n");
      exit(EXIT_FAILURE);
    }

    Span<String> tokens;
    if (bytes_read == 0) {
      tokens = finish(lexer);
    } else {
      String input = slice(buffer, 0, bytes_read);
      tokens       = lex(lexer, input);
    }

    for (String token : tokens) {
      print(INFO "Token: \"", token, "\"\n");
    }
    
    Span<Statement> statements = parse(parser, tokens);
    for (Statement statement : statements) {
      print(statement, '\n');
      validate(statement);
      execute(&arenas[4], statement, database);
    }
  }
}

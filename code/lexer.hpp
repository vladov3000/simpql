struct Lexer {
  Arena* token_arena = nullptr;
  Arena* text_arena  = nullptr;
  String token       = "";
};

static Lexer* make_lexer(Arena* token_arena, Arena* text_arena) {
  Lexer* lexer       = allocate<Lexer>(token_arena);
  lexer->token_arena = token_arena;
  lexer->text_arena  = text_arena;
  lexer->token       = allocate_string(text_arena, 0);
  return lexer;
}

static B32 is_symbol(U8 c) {
  return c == '(' || c == ')' || c == ';' || c == ',';
}

static void flush_token(Lexer* lexer, Span<String>* tokens, String* token) {
  if (token->size > 0) {
    String* token_out = allocate<String>(lexer->token_arena);
    *token_out        = copy(lexer->text_arena, *token);
    tokens->size++;
  }
}

static void add_token(Lexer* lexer, Span<String>* tokens, String* token) {
  if (token->size > 0) {
    String* token_out = allocate<String>(lexer->token_arena);
    *token_out        = *token;
    tokens->size++;

    token->data = allocate_bytes(lexer->text_arena, 0, 1);
    token->size = 0;
  }
}

static Span<String> lex(Lexer* lexer, String input) {
  Arena*       token_arena = lexer->token_arena;
  Arena*       text_arena  = lexer->text_arena;
  Span<String> tokens      = allocate_span<String>(token_arena, 0);
  String       token       = lexer->token;
  for (U8 c : input) {
    if (is_letter(c) || c == '_') {
      *allocate_bytes(text_arena, 1, 1) = c;
      token.size++;
    } else if (is_symbol(c)) {
      add_token(lexer, &tokens, &token);
      token    = allocate_string(text_arena, 1);
      token[0] = c;
      add_token(lexer, &tokens, &token);
    } else if (is_space(c)) {
      add_token(lexer, &tokens, &token);
    } else {
      print(ERROR "Unknown character: '", c, "'.\n");
      exit(EXIT_FAILURE);
    }
  }
  lexer->token = token;
  return tokens;
}

static Span<String> finish(Lexer* lexer) {
  Span<String> tokens;
  if (lexer->token) {
    tokens = allocate_span<String>(lexer->token_arena, 1);
    add_token(lexer, &tokens, &lexer->token);
  } else {
    tokens = Span<String>();
  }
  return tokens;
}

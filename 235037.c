  st_select_lex *outer_select()
  {
    return (outer_context ?
            outer_context->select_lex :
            NULL);
  }
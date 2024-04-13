void promote_select_describe_flag_if_needed(LEX *lex)
{
  if (lex->describe)
  {
    lex->select_lex.options |= SELECT_DESCRIBE;
  }
}
void lex_end(LEX *lex)
{
  DBUG_ENTER("lex_end");
  DBUG_PRINT("enter", ("lex: %p", lex));

  lex_unlock_plugins(lex);
  lex_end_nops(lex);

  DBUG_VOID_RETURN;
}
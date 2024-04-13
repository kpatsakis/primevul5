void lex_end_nops(LEX *lex)
{
  DBUG_ENTER("lex_end_nops");
  sp_head::destroy(lex->sphead);
  lex->sphead= NULL;

  /* Reset LEX_MASTER_INFO */
  lex->mi.reset(lex->sql_command == SQLCOM_CHANGE_MASTER);
  delete_dynamic(&lex->delete_gtid_domain);

  DBUG_VOID_RETURN;
}
init_lex_with_single_table(THD *thd, TABLE *table, LEX *lex)
{
  TABLE_LIST *table_list;
  Table_ident *table_ident;
  SELECT_LEX *select_lex= &lex->select_lex;
  Name_resolution_context *context= &select_lex->context;
  /*
    We will call the parser to create a part_info struct based on the
    partition string stored in the frm file.
    We will use a local lex object for this purpose. However we also
    need to set the Name_resolution_object for this lex object. We
    do this by using add_table_to_list where we add the table that
    we're working with to the Name_resolution_context.
  */
  thd->lex= lex;
  lex_start(thd);
  context->init();
  if ((!(table_ident= new Table_ident(thd,
                                      table->s->table_name,
                                      table->s->db, TRUE))) ||
      (!(table_list= select_lex->add_table_to_list(thd,
                                                   table_ident,
                                                   NULL,
                                                   0))))
    return TRUE;
  context->resolve_in_table_list_only(table_list);
  lex->use_only_table_context= TRUE;
  lex->context_analysis_only|= CONTEXT_ANALYSIS_ONLY_VCOL_EXPR;
  select_lex->cur_pos_in_select_list= UNDEF_POS;
  table->map= 1; //To ensure correct calculation of const item
  table_list->table= table;
  table_list->cacheable_table= false;
  return FALSE;
}
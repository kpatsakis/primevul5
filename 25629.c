bool JOIN::prepare_result(List<Item> **columns_list)
{
  DBUG_ENTER("JOIN::prepare_result");

  error= 0;
  /* Create result tables for materialized views. */
  if (!zero_result_cause &&
      select_lex->handle_derived(thd->lex, DT_CREATE))
    goto err;

  if (result->prepare2())
    goto err;

  if ((select_lex->options & OPTION_SCHEMA_TABLE) &&
      get_schema_tables_result(this, PROCESSED_BY_JOIN_EXEC))
    goto err;

  DBUG_RETURN(FALSE);

err:
  error= 1;
  DBUG_RETURN(TRUE);
}
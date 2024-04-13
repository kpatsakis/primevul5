bool JOIN::prepare_stage2()
{
  bool res= TRUE;
  DBUG_ENTER("JOIN::prepare_stage2");

  /* Init join struct */
  count_field_types(select_lex, &tmp_table_param, all_fields, 0);
  this->group= group_list != 0;

  if (tmp_table_param.sum_func_count && !group_list)
  {
    implicit_grouping= TRUE;
    // Result will contain zero or one row - ordering is meaningless
    order= NULL;
  }

#ifdef RESTRICTED_GROUP
  if (implicit_grouping)
  {
    my_message(ER_WRONG_SUM_SELECT,ER_THD(thd, ER_WRONG_SUM_SELECT),MYF(0));
    goto err;
  }
#endif
  if (select_lex->olap == ROLLUP_TYPE && rollup_init())
    goto err;
  if (alloc_func_list())
    goto err;

  res= FALSE;
err:
  DBUG_RETURN(res);				/* purecov: inspected */
}
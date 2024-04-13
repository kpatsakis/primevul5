static void select_describe(JOIN *join, bool need_tmp_table, bool need_order,
			    bool distinct,const char *message)
{
  THD *thd=join->thd;
  select_result *result=join->result;
  DBUG_ENTER("select_describe");
  
  /* Update the QPF with latest values of using_temporary, using_filesort */
  for (SELECT_LEX_UNIT *unit= join->select_lex->first_inner_unit();
       unit;
       unit= unit->next_unit())
  {
    /*
      This fix_fields() call is to handle an edge case like this:
       
        SELECT ... UNION SELECT ... ORDER BY (SELECT ...)
      
      for such queries, we'll get here before having called
      subquery_expr->fix_fields(), which will cause failure to
    */
    if (unit->item && !unit->item->fixed)
    {
      Item *ref= unit->item;
      if (unit->item->fix_fields(thd, &ref))
        DBUG_VOID_RETURN;
      DBUG_ASSERT(ref == unit->item);
    }

    /* 
      Save plans for child subqueries, when
      (1) they are not parts of eliminated WHERE/ON clauses.
      (2) they are not VIEWs that were "merged for INSERT".
      (3) they are not hanging CTEs (they are needed for execution)
    */
    if (!(unit->item && unit->item->eliminated) &&                     // (1)
        !(unit->derived && unit->derived->merged_for_insert) &&        // (2)
        (!unit->with_element ||
          (unit->derived &&
           unit->derived->derived_result &&
           !unit->with_element->is_hanging_recursive())))              // (3)
    {
      if (mysql_explain_union(thd, unit, result))
        DBUG_VOID_RETURN;
    }
  }
  DBUG_VOID_RETURN;
}
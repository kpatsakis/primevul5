void remove_redundant_subquery_clauses(st_select_lex *subq_select_lex)
{
  DBUG_ENTER("remove_redundant_subquery_clauses");
  Item_subselect *subq_predicate= subq_select_lex->master_unit()->item;
  /*
    The removal should happen for IN, ALL, ANY and EXISTS subqueries,
    which means all but single row subqueries. Example single row
    subqueries: 
       a) SELECT * FROM t1 WHERE t1.a = (<single row subquery>) 
       b) SELECT a, (<single row subquery) FROM t1
   */
  if (subq_predicate->substype() == Item_subselect::SINGLEROW_SUBS)
    DBUG_VOID_RETURN;

  /* A subquery that is not single row should be one of IN/ALL/ANY/EXISTS. */
  DBUG_ASSERT (subq_predicate->substype() == Item_subselect::EXISTS_SUBS ||
               subq_predicate->is_in_predicate());

  if (subq_select_lex->options & SELECT_DISTINCT)
  {
    subq_select_lex->join->select_distinct= false;
    subq_select_lex->options&= ~SELECT_DISTINCT;
    DBUG_PRINT("info", ("DISTINCT removed"));
  }

  /*
    Remove GROUP BY if there are no aggregate functions and no HAVING
    clause
  */
  if (subq_select_lex->group_list.elements &&
      !subq_select_lex->with_sum_func && !subq_select_lex->join->having)
  {
    for (ORDER *ord= subq_select_lex->group_list.first; ord; ord= ord->next)
    {
      /*
        Do not remove the item if it is used in select list and then referred
        from GROUP BY clause by its name or number. Example:

          select (select ... ) as SUBQ ...  group by SUBQ

        Here SUBQ cannot be removed.
      */
      if (!ord->in_field_list)
        (*ord->item)->walk(&Item::eliminate_subselect_processor, FALSE, NULL);
    }
    subq_select_lex->join->group_list= NULL;
    subq_select_lex->group_list.empty();
    DBUG_PRINT("info", ("GROUP BY removed"));
  }

  /*
    TODO: This would prevent processing quries with ORDER BY ... LIMIT
    therefore we disable this optimization for now.
    Remove GROUP BY if there are no aggregate functions and no HAVING
    clause
  if (subq_select_lex->group_list.elements &&
      !subq_select_lex->with_sum_func && !subq_select_lex->join->having)
  {
    subq_select_lex->join->group_list= NULL;
    subq_select_lex->group_list.empty();
  }
  */
  DBUG_VOID_RETURN;
}
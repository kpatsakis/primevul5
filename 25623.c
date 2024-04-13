bool JOIN::setup_subquery_caches()
{
  DBUG_ENTER("JOIN::setup_subquery_caches");

  /*
    We have to check all this condition together because items created in
    one of this clauses can be moved to another one by optimizer
  */
  if (select_lex->expr_cache_may_be_used[IN_WHERE] ||
      select_lex->expr_cache_may_be_used[IN_HAVING] ||
      select_lex->expr_cache_may_be_used[IN_ON] ||
      select_lex->expr_cache_may_be_used[NO_MATTER])
  {
    if (conds)
      conds= conds->transform(thd, &Item::expr_cache_insert_transformer,
                              NULL);
    JOIN_TAB *tab;
    for (tab= first_linear_tab(this, WITH_BUSH_ROOTS, WITHOUT_CONST_TABLES);
         tab; tab= next_linear_tab(this, tab, WITH_BUSH_ROOTS))
    {
      if (tab->select_cond)
        tab->select_cond=
          tab->select_cond->transform(thd, &Item::expr_cache_insert_transformer,
                                      NULL);
      if (tab->cache_select && tab->cache_select->cond)
        tab->cache_select->cond=
          tab->cache_select->
          cond->transform(thd, &Item::expr_cache_insert_transformer,
                          NULL);

    }

    if (having)
      having= having->transform(thd, &Item::expr_cache_insert_transformer,
                                NULL);
    if (tmp_having)
    {
      DBUG_ASSERT(having == NULL);
      tmp_having= tmp_having->transform(thd, &Item::expr_cache_insert_transformer,
                                        NULL);
    }
  }
  if (select_lex->expr_cache_may_be_used[SELECT_LIST] ||
      select_lex->expr_cache_may_be_used[IN_GROUP_BY] ||
      select_lex->expr_cache_may_be_used[NO_MATTER])
  {
    List_iterator<Item> li(all_fields);
    Item *item;
    while ((item= li++))
    {
      Item *new_item=
        item->transform(thd, &Item::expr_cache_insert_transformer,
                        NULL);
      if (new_item != item)
      {
        thd->change_item_tree(li.ref(), new_item);
      }
    }
    for (ORDER *tmp_group= group_list; tmp_group ; tmp_group= tmp_group->next)
    {
      *tmp_group->item=
        (*tmp_group->item)->transform(thd, &Item::expr_cache_insert_transformer,
                                      NULL);
    }
  }
  if (select_lex->expr_cache_may_be_used[NO_MATTER])
  {
    for (ORDER *ord= order; ord; ord= ord->next)
    {
      *ord->item=
        (*ord->item)->transform(thd, &Item::expr_cache_insert_transformer,
                                NULL);
    }
  }
  DBUG_RETURN(FALSE);
}
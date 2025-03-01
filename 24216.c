int init_ftfuncs(THD *thd, SELECT_LEX *select_lex, bool no_order)
{
  if (select_lex->ftfunc_list->elements)
  {
    List_iterator<Item_func_match> li(*(select_lex->ftfunc_list));
    Item_func_match *ifm;
    DBUG_PRINT("info",("Performing FULLTEXT search"));

    while ((ifm=li++))
      if (unlikely(!ifm->fixed))
        /*
          it mean that clause where was FT function was removed, so we have
          to remove the function from the list.
        */
        li.remove();
      else
        ifm->init_search(thd, no_order);
  }
  return 0;
}
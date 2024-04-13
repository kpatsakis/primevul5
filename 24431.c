bool st_select_lex::mark_as_dependent(THD *thd, st_select_lex *last,
                                      Item_ident *dependency)
{

  DBUG_ASSERT(this != last);

  /*
    Mark all selects from resolved to 1 before select where was
    found table as depended (of select where was found table)

    We move by name resolution context, bacause during merge can some select
    be excleded from SELECT tree
  */
  Name_resolution_context *c= &this->context;
  do
  {
    SELECT_LEX *s= c->select_lex;
    if (!(s->uncacheable & UNCACHEABLE_DEPENDENT_GENERATED))
    {
      // Select is dependent of outer select
      s->uncacheable= (s->uncacheable & ~UNCACHEABLE_UNITED) |
                       UNCACHEABLE_DEPENDENT_GENERATED;
      SELECT_LEX_UNIT *munit= s->master_unit();
      munit->uncacheable= (munit->uncacheable & ~UNCACHEABLE_UNITED) |
                       UNCACHEABLE_DEPENDENT_GENERATED;
      for (SELECT_LEX *sl= munit->first_select(); sl ; sl= sl->next_select())
      {
        if (sl != s &&
            !(sl->uncacheable & (UNCACHEABLE_DEPENDENT_GENERATED |
                                 UNCACHEABLE_UNITED)))
          sl->uncacheable|= UNCACHEABLE_UNITED;
      }
    }

    Item_subselect *subquery_expr= s->master_unit()->item;
    if (subquery_expr && subquery_expr->mark_as_dependent(thd, last, 
                                                          dependency))
      return TRUE;
  } while ((c= c->outer_context) != NULL && (c->select_lex != last));
  is_correlated= TRUE;
  this->master_unit()->item->is_correlated= TRUE;
  return FALSE;
}
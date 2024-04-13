void Item_func_nullif::print(String *str, enum_query_type query_type)
{
  /*
    NULLIF(a,b) is implemented according to the SQL standard as a short for
    CASE WHEN a=b THEN NULL ELSE a END

    The constructor of Item_func_nullif sets args[0] and args[2] to the
    same item "a", and sets args[1] to "b".

    If "this" is a part of a WHERE or ON condition, then:
    - the left "a" is a subject to equal field propagation with ANY_SUBST.
    - the right "a" is a subject to equal field propagation with IDENTITY_SUBST.
    Therefore, after equal field propagation args[0] and args[2] can point
    to different items.
  */
  if ((query_type & QT_ITEM_ORIGINAL_FUNC_NULLIF) ||
      (arg_count == 2) ||
      (args[0] == args[2]))
  {
    /*
      If QT_ITEM_ORIGINAL_FUNC_NULLIF is requested,
      that means we want the original NULLIF() representation,
      e.g. when we are in:
        SHOW CREATE {VIEW|FUNCTION|PROCEDURE}

      The original representation is possible only if
      args[0] and args[2] still point to the same Item.

      The caller must never pass call print() with QT_ITEM_ORIGINAL_FUNC_NULLIF
      if an expression has undergone some optimization
      (e.g. equal field propagation done in optimize_cond()) already and
      NULLIF() potentially has two different representations of "a":
      - one "a" for comparison
      - another "a" for the returned value!
    */
    DBUG_ASSERT(arg_count == 2 ||
                args[0] == args[2] || current_thd->lex->context_analysis_only);
    str->append(func_name());
    str->append('(');
    if (arg_count == 2)
      args[0]->print(str, query_type);
    else
      args[2]->print(str, query_type);
    str->append(',');
    args[1]->print(str, query_type);
    str->append(')');
  }
  else
  {
    /*
      args[0] and args[2] are different items.
      This is possible after WHERE optimization (equal fields propagation etc),
      e.g. in EXPLAIN EXTENDED or EXPLAIN FORMAT=JSON.
      As it's not possible to print as a function with 2 arguments any more,
      do it in the CASE style.
    */
    str->append(STRING_WITH_LEN("(case when "));
    args[0]->print(str, query_type);
    str->append(STRING_WITH_LEN(" = "));
    args[1]->print(str, query_type);
    str->append(STRING_WITH_LEN(" then NULL else "));
    args[2]->print(str, query_type);
    str->append(STRING_WITH_LEN(" end)"));
  }
}
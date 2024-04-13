Item_func_nullif::fix_length_and_dec()
{
  /*
    If this is the first invocation of fix_length_and_dec(), create the
    third argument as a copy of the first. This cannot be done before
    fix_fields(), because fix_fields() might replace items,
    for exampe NOT x --> x==0, or (SELECT 1) --> 1.
    See also class Item_func_nullif declaration.
  */
  if (arg_count == 2)
    args[arg_count++]= m_arg0 ? m_arg0 : args[0];

  THD *thd= current_thd;
  /*
    At prepared statement EXECUTE time, args[0] can already
    point to a different Item, created during PREPARE time fix_length_and_dec().
    For example, if character set conversion was needed, arguments can look
    like this:

      args[0]= > Item_func_conv_charset \
                                         l_expr
      args[2]= >------------------------/

    Otherwise (during PREPARE or convensional execution),
    args[0] and args[2] should still point to the same original l_expr.
  */
  DBUG_ASSERT(args[0] == args[2] || thd->stmt_arena->is_stmt_execute());
  if (args[0]->type() == SUM_FUNC_ITEM &&
      !thd->lex->is_ps_or_view_context_analysis())
  {
    /*
      NULLIF(l_expr, r_expr)

        is calculated in the way to return a result equal to:

      CASE WHEN l_expr = r_expr THEN NULL ELSE r_expr END.

      There's nothing special with r_expr, because it's referenced
      only by args[1] and nothing else.

      l_expr needs a special treatment, as it's referenced by both
      args[0] and args[2] initially.

      args[2] is used to return the value. Afrer all transformations
      (e.g. in fix_length_and_dec(), equal field propagation, etc)
      args[2] points to a an Item which preserves the exact data type and
      attributes (e.g. collation) of the original l_expr.
      It can point:
      - to the original l_expr
      - to an Item_cache pointing to l_expr
      - to a constant of the same data type with l_expr.

      args[0] is used for comparison. It can be replaced:

      - to Item_func_conv_charset by character set aggregation routines
      - to a constant Item by equal field propagation routines
        (in case of Item_field)

      The data type and/or the attributes of args[0] can differ from
      the data type and the attributes of the original l_expr, to make
      it comparable to args[1] (which points to r_expr or its replacement).

      For aggregate functions we have to wrap the original args[0]/args[2]
      into Item_cache (see MDEV-9181). In this case the Item_cache
      instance becomes the subject to character set conversion instead of
      the original args[0]/args[2], while the original args[0]/args[2] get
      hidden inside the cache.

      Some examples of what NULLIF can end up with after argument
      substitution (we don't mention args[1] in some cases for simplicity):

      1. l_expr is not an aggregate function:

        a. No conversion happened.
           args[0] and args[2] were not replaced to something else
           (i.e. neither by character set conversion, nor by propagation):

          args[1] > r_expr
          args[0] \
                    l_expr
          args[2] /

        b. Conversion of args[0] happened:

           CREATE OR REPLACE TABLE t1 (
             a CHAR(10) CHARACTER SET latin1,
             b CHAR(10) CHARACTER SET utf8);
           SELECT * FROM t1 WHERE NULLIF(a,b);

           args[1] > r_expr                          (Item_field for t1.b)
           args[0] > Item_func_conv_charset\
                                            l_expr   (Item_field for t1.a)
           args[2] > ----------------------/

        c. Conversion of args[1] happened:

          CREATE OR REPLACE TABLE t1 (
            a CHAR(10) CHARACTER SET utf8,
            b CHAR(10) CHARACTER SET latin1);
          SELECT * FROM t1 WHERE NULLIF(a,b);

          args[1] > Item_func_conv_charset -> r_expr (Item_field for t1.b)
          args[0] \
                   l_expr                            (Item_field for t1.a)
          args[2] /

        d. Conversion of only args[0] happened (by equal field proparation):

           CREATE OR REPLACE TABLE t1 (
             a CHAR(10),
             b CHAR(10));
           SELECT * FROM t1 WHERE NULLIF(a,b) AND a='a';

           args[1] > r_expr            (Item_field for t1.b)
           args[0] > Item_string('a')  (constant replacement for t1.a)
           args[2] > l_expr            (Item_field for t1.a)

        e. Conversion of both args[0] and args[2] happened
           (by equal field propagation):

           CREATE OR REPLACE TABLE t1 (a INT,b INT);
           SELECT * FROM t1 WHERE NULLIF(a,b) AND a=5;

           args[1] > r_expr         (Item_field for "b")
           args[0] \
                    Item_int (5)    (constant replacement for "a")
           args[2] /

      2. In case if l_expr is an aggregate function:

        a. No conversion happened:

          args[0] \
                   Item_cache > l_expr
          args[2] /

        b. Conversion of args[0] happened:

          args[0] > Item_func_conv_charset \
                                            Item_cache > l_expr
          args[2] >------------------------/

        c. Conversion of both args[0] and args[2] happened.
           (e.g. by equal expression propagation)
           TODO: check if it's possible (and add an example query if so).
    */
    m_cache= args[0]->cmp_type() == STRING_RESULT ?
             new (thd->mem_root) Item_cache_str_for_nullif(thd, args[0]) :
             args[0]->get_cache(thd);
    if (!m_cache)
      return TRUE;
    m_cache->setup(thd, args[0]);
    m_cache->store(args[0]);
    m_cache->set_used_tables(args[0]->used_tables());
    thd->change_item_tree(&args[0], m_cache);
    thd->change_item_tree(&args[2], m_cache);
  }
  set_handler(args[2]->type_handler());
  collation.set(args[2]->collation);
  decimals= args[2]->decimals;
  unsigned_flag= args[2]->unsigned_flag;
  fix_char_length(args[2]->max_char_length());
  maybe_null=1;
  m_arg0= args[0];
  if (setup_args_and_comparator(thd, &cmp))
    return TRUE;
  /*
    A special code for EXECUTE..PREPARE.

    If args[0] did not change, then we don't remember it, as it can point
    to a temporary Item object which will be destroyed between PREPARE
    and EXECUTE. EXECUTE time fix_length_and_dec() will correctly set args[2]
    from args[0] again.

    If args[0] changed, then it can be Item_func_conv_charset() for the
    original args[0], which was permanently installed during PREPARE time
    into the item tree as a wrapper for args[0], using change_item_tree(), i.e.

      NULLIF(latin1_field, 'a' COLLATE utf8_bin)

    was "rewritten" to:

      CASE WHEN CONVERT(latin1_field USING utf8) = 'a' COLLATE utf8_bin
        THEN NULL
        ELSE latin1_field

    - m_args0 points to Item_field corresponding to latin1_field
    - args[0] points to Item_func_conv_charset
    - args[0]->args[0] is equal to m_args0
    - args[1] points to Item_func_set_collation
    - args[2] points is eqial to m_args0

    In this case we remember and reuse m_arg0 during EXECUTE time as args[2].

    QQ: How to make sure that m_args0 does not point
    to something temporary which will be destroyed between PREPARE and EXECUTE.
    The condition below should probably be more strict and somehow check that:
    - change_item_tree() was called for the new args[0]
    - m_args0 is referenced from inside args[0], e.g. as a function argument,
      and therefore it is also something that won't be destroyed between
      PREPARE and EXECUTE.
    Any ideas?
  */
  if (args[0] == m_arg0)
    m_arg0= NULL;
  return FALSE;
}
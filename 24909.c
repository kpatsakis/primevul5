void Item_equal::add_const(THD *thd, Item *c)
{
  if (cond_false)
    return;
  if (!with_const)
  {
    with_const= TRUE;
    equal_items.push_front(c, thd->mem_root);
    return;
  }

  /*
    Suppose we have an expression (with a string type field) like this:
      WHERE field=const1 AND field=const2 ...

    For all pairs field=constXXX we know that:

    - Item_func_eq::fix_length_and_dec() performed collation and character
    set aggregation and added character set converters when needed.
    Note, the case like:
      WHERE field=const1 COLLATE latin1_bin AND field=const2
    is not handled here, because the field would be replaced to
    Item_func_set_collation, which cannot get into Item_equal.
    So all constXXX that are handled by Item_equal
    already have compatible character sets with "field".

    - Also, Field_str::test_if_equality_guarantees_uniqueness() guarantees
    that the comparison collation of all equalities handled by Item_equal
    match the the collation of the field.

    Therefore, at Item_equal::add_const() time all constants constXXX
    should be directly comparable to each other without an additional
    character set conversion.
    It's safe to do val_str() for "const_item" and "c" and compare
    them according to the collation of the *field*.

    So in a script like this:
      CREATE TABLE t1 (a VARCHAR(10) COLLATE xxx);
      INSERT INTO t1 VALUES ('a'),('A');
      SELECT * FROM t1 WHERE a='a' AND a='A';
    Item_equal::add_const() effectively rewrites the condition to:
      SELECT * FROM t1 WHERE a='a' AND 'a' COLLATE xxx='A';
    and then to:
      SELECT * FROM t1 WHERE a='a'; // if the two constants were equal
                                    // e.g. in case of latin1_swedish_ci
    or to:
      SELECT * FROM t1 WHERE FALSE; // if the two constants were not equal
                                    // e.g. in case of latin1_bin

    Note, both "const_item" and "c" can return NULL, e.g.:
      SELECT * FROM t1 WHERE a=NULL    AND a='const';
      SELECT * FROM t1 WHERE a='const' AND a=NULL;
      SELECT * FROM t1 WHERE a='const' AND a=(SELECT MAX(a) FROM t2)
  */

  cond_false= !Item_equal::compare_type_handler()->Item_eq_value(thd, this, c,
                                                                 get_const());
  if (with_const && equal_items.elements == 1)
    cond_true= TRUE;
  if (cond_false || cond_true)
    const_item_cache= 1;
}
can_change_cond_ref_to_const(Item_bool_func2 *target,
                             Item *target_expr, Item *target_value,
                             Item_bool_func2 *source,
                             Item *source_expr, Item *source_const)
{
  if (!target_expr->eq(source_expr,0) ||
       target_value == source_const ||
       target->compare_type() != source->compare_type())
    return false;
  if (target->compare_type() == STRING_RESULT)
  {
    /*
      In this example:
        SET NAMES utf8 COLLATE utf8_german2_ci;
        DROP TABLE IF EXISTS t1;
        CREATE TABLE t1 (a CHAR(10) CHARACTER SET utf8);
        INSERT INTO t1 VALUES ('o-umlaut'),('oe');
        SELECT * FROM t1 WHERE a='oe' COLLATE utf8_german2_ci AND a='oe';

      the query should return only the row with 'oe'.
      It should not return 'o-umlaut', because 'o-umlaut' does not match
      the right part of the condition: a='oe'
      ('o-umlaut' is not equal to 'oe' in utf8_general_ci,
       which is the collation of the field "a").

      If we change the right part from:
         ... AND a='oe'
      to
         ... AND 'oe' COLLATE utf8_german2_ci='oe'
      it will be evalulated to TRUE and removed from the condition,
      so the overall query will be simplified to:

        SELECT * FROM t1 WHERE a='oe' COLLATE utf8_german2_ci;

      which will erroneously start to return both 'oe' and 'o-umlaut'.
      So changing "expr" to "const" is not possible if the effective
      collations of "target" and "source" are not exactly the same.

      Note, the code before the fix for MDEV-7152 only checked that
      collations of "source_const" and "target_value" are the same.
      This was not enough, as the bug report demonstrated.
    */
    return
      target->compare_collation() == source->compare_collation() &&
      target_value->collation.collation == source_const->collation.collation;
  }
  if (target->compare_type() == TIME_RESULT)
  {
    if (target_value->cmp_type() != TIME_RESULT)
    {
      /*
        Can't rewrite:
          WHERE COALESCE(time_column)='00:00:00'
            AND COALESCE(time_column)=DATE'2015-09-11'
        to
          WHERE DATE'2015-09-11'='00:00:00'
            AND COALESCE(time_column)=DATE'2015-09-11'
        because the left part will erroneously try to parse '00:00:00'
        as DATE, not as TIME.

        TODO: It could still be rewritten to:
          WHERE DATE'2015-09-11'=TIME'00:00:00'
            AND COALESCE(time_column)=DATE'2015-09-11'
        i.e. we need to replace both target_expr and target_value
        at the same time. This is not supported yet.
      */
      return false;
    }
  }
  return true; // Non-string comparison
}
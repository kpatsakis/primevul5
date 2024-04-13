bool Predicant_to_list_comparator::add_value_skip_null(const char *funcname,
                                                       Item_args *args,
                                                       uint value_index,
                                                       bool *nulls_found)
{
  /*
    Skip explicit NULL constant items.
    Using real_item() to correctly detect references to explicit NULLs
    in HAVING clause, e.g. in this example "b" is skipped:
      SELECT a,NULL AS b FROM t1 GROUP BY a HAVING 'A' IN (b,'A');
  */
  if (args->arguments()[value_index]->real_item()->type() == Item::NULL_ITEM)
  {
    *nulls_found= true;
    return false;
  }
  return add_value(funcname, args, value_index);
}
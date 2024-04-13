static bool are_tables_local(JOIN_TAB *jtab, table_map used_tables)
{
  if (jtab->bush_root_tab)
  {
    /*
      jtab is inside execution join nest. We may not refer to outside tables,
      except the const tables.
    */
    table_map local_tables= jtab->emb_sj_nest->nested_join->used_tables |
                            jtab->join->const_table_map |
                            OUTER_REF_TABLE_BIT;
    return !MY_TEST(used_tables & ~local_tables);
  }

  /* 
    If we got here then jtab is at top level. 
     - all other tables at top level are accessible,
     - tables in join nests are accessible too, because all their columns that 
       are needed at top level will be unpacked when scanning the
       materialization table.
  */
  return TRUE;
}
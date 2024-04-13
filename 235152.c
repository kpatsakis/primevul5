  bool check_table_name_processor(void *arg)
  {
    Check_table_name_prm &p= *(Check_table_name_prm *) arg;
    if (p.table_name.length && table_name)
    {
      DBUG_ASSERT(p.db.length);
      if ((db_name &&
          my_strcasecmp(table_alias_charset, p.db.str, db_name)) ||
          my_strcasecmp(table_alias_charset, p.table_name.str, table_name))
      {
        print(&p.field, (enum_query_type) (QT_ITEM_ORIGINAL_FUNC_NULLIF |
                                          QT_NO_DATA_EXPANSION |
                                          QT_TO_SYSTEM_CHARSET));
        return true;
      }
    }
    return false;
  }
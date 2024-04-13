  bool excl_dep_on_table(table_map tab_map)
  {
    for (uint i= 0; i < arg_count; i++)
    {
      if (args[i]->const_item())
        continue;
      if (!args[i]->excl_dep_on_table(tab_map))
        return false;
    }
    return true;
  }
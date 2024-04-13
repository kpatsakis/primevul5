  bool excl_dep_on_grouping_fields(st_select_lex *sel)
  {
    for (uint i= 0; i < arg_count; i++)
    {
      if (args[i]->const_item())
        continue;      
      if (!args[i]->excl_dep_on_grouping_fields(sel))
        return false;
    }
    return true;
  }
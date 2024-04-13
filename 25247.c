  bool excl_dep_on_in_subq_left_part(Item_in_subselect *subq_pred)
  {
    for (uint i= 0; i < arg_count; i++)
    {
      if (args[i]->const_item())
        continue;
      if (!args[i]->excl_dep_on_in_subq_left_part(subq_pred))
        return false;
    }
    return true;
  }
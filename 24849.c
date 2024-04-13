  bool eq(const Item_args *other, bool binary_cmp) const
  {
    for (uint i= 0; i < arg_count ; i++)
    {
      if (!args[i]->eq(other->args[i], binary_cmp))
        return false;
    }
    return true;
  }
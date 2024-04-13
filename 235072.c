  bool check_cols(uint c)
  {
    return (result_type() == ROW_RESULT ?
            orig_item->check_cols(c) :
            Item::check_cols(c));
  }
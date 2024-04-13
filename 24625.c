  bool check_cols(uint c)
  {
    if (cols() != c)
    {
      my_error(ER_OPERAND_COLUMNS, MYF(0), c);
      return true;
    }
    return false;
  }
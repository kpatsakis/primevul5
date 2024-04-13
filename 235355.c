  virtual Item* compile(THD *thd, Item_analyzer analyzer, uchar **arg_p,
                        Item_transformer transformer, uchar *arg_t)
  {
    if ((this->*analyzer) (arg_p))
      return ((this->*transformer) (thd, arg_t));
    return 0;
  }
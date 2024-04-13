  Item_args(THD *thd, Item *a, Item *b, Item *c)
  {
    arg_count= 0;
    if ((args= (Item**) thd_alloc(thd, sizeof(Item*) * 3)))
    {
      arg_count= 3;
      args[0]= a; args[1]= b; args[2]= c;
    }
  }
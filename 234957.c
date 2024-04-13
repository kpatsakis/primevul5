  Item_args(THD *thd, Item *a, Item *b, Item *c, Item *d)
  {
    arg_count= 0;
    if ((args= (Item**) thd_alloc(thd, sizeof(Item*) * 4)))
    {
      arg_count= 4;
      args[0]= a; args[1]= b; args[2]= c; args[3]= d;
    }
  }
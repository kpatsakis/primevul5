  Item_args(THD *thd, Item *a, Item *b, Item *c, Item *d, Item* e)
  {
    arg_count= 5;
    if ((args= (Item**) thd_alloc(thd, sizeof(Item*) * 5)))
    {
      arg_count= 5;
      args[0]= a; args[1]= b; args[2]= c; args[3]= d; args[4]= e;
    }
  }
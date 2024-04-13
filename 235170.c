  Item_null(THD *thd, char *name_par=0, CHARSET_INFO *cs= &my_charset_bin):
    Item_basic_constant(thd)
  {
    maybe_null= null_value= TRUE;
    max_length= 0;
    name= name_par ? name_par : (char*) "NULL";
    fixed= 1;
    collation.set(cs, DERIVATION_IGNORABLE, MY_REPERTOIRE_ASCII);
  }
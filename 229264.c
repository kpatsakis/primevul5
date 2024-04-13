my_bool _mariadb_set_conf_option(MYSQL *mysql, const char *config_option, const char *config_value)
{
  if (config_option)
  {
    int i;
    char *c;
    
    /* CONC-395: replace underscore "_" by dash "-" */
    while ((c= strchr(config_option, '_')))
      *c= '-';

    for (i=0; mariadb_defaults[i].conf_key; i++)
    {
      if (!strcmp(mariadb_defaults[i].conf_key, config_option))
      {
        my_bool val_bool;
        int     val_int;
        size_t  val_sizet;
        int rc;
        void *option_val= NULL;
        switch (mariadb_defaults[i].type) {
        case MARIADB_OPTION_BOOL:
          val_bool= 0;
          if (config_value)
            val_bool= atoi(config_value);
          option_val= &val_bool;
          break;
        case MARIADB_OPTION_INT:
          val_int= 0;
          if (config_value)
            val_int= atoi(config_value);
          option_val= &val_int;
          break;
        case MARIADB_OPTION_SIZET:
          val_sizet= 0;
          if (config_value)
            val_sizet= strtol(config_value, NULL, 10);
          option_val= &val_sizet;
          break;
        case MARIADB_OPTION_STR:
          option_val= (void*)config_value;
          break;
        case MARIADB_OPTION_NONE:
          break;
        }
        rc= mysql_optionsv(mysql, mariadb_defaults[i].option, option_val);
        return(test(rc));
      }
    }
  }
  /* unknown key */
  return 1;
}
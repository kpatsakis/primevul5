static void options_add_initcommand(struct st_mysql_options *options,
                                     const char *init_cmd)
{
  char *insert= strdup(init_cmd);
  if (!options->init_command)
  {
    options->init_command= (DYNAMIC_ARRAY*)malloc(sizeof(DYNAMIC_ARRAY));
    ma_init_dynamic_array(options->init_command, sizeof(char*), 5, 5);
  }

  if (ma_insert_dynamic(options->init_command, (gptr)&insert))
    free(insert);
}
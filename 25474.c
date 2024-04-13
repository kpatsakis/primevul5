void add_plugin_options(DYNAMIC_ARRAY *options, MEM_ROOT *mem_root)
{
  struct st_plugin_int *p;
  my_option *opt;

  if (!initialized)
    return;

  for (uint idx= 0; idx < plugin_array.elements; idx++)
  {
    p= *dynamic_element(&plugin_array, idx, struct st_plugin_int **);

    if (!(opt= construct_help_options(mem_root, p)))
      continue;

    /* Only options with a non-NULL comment are displayed in help text */
    for (;opt->name; opt++)
      if (opt->comment)
        insert_dynamic(options, (uchar*) opt);
  }
}
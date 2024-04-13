read_pattern_file (void)
{
  char **new_save_patterns = NULL;
  size_t max_new_patterns;
  size_t new_num_patterns;
  int i;
  dynamic_string pattern_name = DYNAMIC_STRING_INITIALIZER;
  FILE *pattern_fp;

  if (num_patterns < 0)
    num_patterns = 0;
  new_num_patterns = num_patterns;
  max_new_patterns = num_patterns;
  new_save_patterns = xcalloc (max_new_patterns, sizeof (new_save_patterns[0]));

  pattern_fp = fopen (pattern_file_name, "r");
  if (pattern_fp == NULL)
    open_fatal (pattern_file_name);
  while (ds_fgetstr (pattern_fp, &pattern_name, '\n') != NULL)
    {
      if (new_num_patterns == max_new_patterns)
	new_save_patterns = x2nrealloc (new_save_patterns,
					&max_new_patterns,
					sizeof (new_save_patterns[0]));
      new_save_patterns[new_num_patterns] = xstrdup (pattern_name.ds_string);
      ++new_num_patterns;
    }

  ds_free (&pattern_name);
  
  if (ferror (pattern_fp) || fclose (pattern_fp) == EOF)
    close_error (pattern_file_name);

  for (i = 0; i < num_patterns; ++i)
    new_save_patterns[i] = save_patterns[i];

  save_patterns = new_save_patterns;
  num_patterns = new_num_patterns;
}
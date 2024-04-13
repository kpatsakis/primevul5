add_temp_array_to_env (temp_array, do_alloc, do_supercede)
     char **temp_array;
     int do_alloc, do_supercede;
{
  register int i;

  if (temp_array == 0)
    return;

  for (i = 0; temp_array[i]; i++)
    {
      if (do_supercede)
	export_env = add_or_supercede_exported_var (temp_array[i], do_alloc);
      else
	add_to_export_env (temp_array[i], do_alloc);
    }

  free (temp_array);
}
put_gnu_argv_flags_into_env (pid, flags_string)
     intmax_t pid;
     char *flags_string;
{
  char *dummy, *pbuf;
  int l, fl;

  pbuf = itos (pid);
  l = strlen (pbuf);

  fl = strlen (flags_string);

  dummy = (char *)xmalloc (l + fl + 30);
  dummy[0] = '_';
  strcpy (dummy + 1, pbuf);
  strcpy (dummy + 1 + l, "_GNU_nonoption_argv_flags_");
  dummy[l + 27] = '=';
  strcpy (dummy + l + 28, flags_string);

  free (pbuf);

  export_env = add_or_supercede_exported_var (dummy, 0);
}
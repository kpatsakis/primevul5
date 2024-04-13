make_vers_array ()
{
  SHELL_VAR *vv;
  ARRAY *av;
  char *s, d[32], b[INT_STRLEN_BOUND(int) + 1];

  unbind_variable ("BASH_VERSINFO");

  vv = make_new_array_variable ("BASH_VERSINFO");
  av = array_cell (vv);
  strcpy (d, dist_version);
  s = strchr (d, '.');
  if (s)
    *s++ = '\0';
  array_insert (av, 0, d);
  array_insert (av, 1, s);
  s = inttostr (patch_level, b, sizeof (b));
  array_insert (av, 2, s);
  s = inttostr (build_version, b, sizeof (b));
  array_insert (av, 3, s);
  array_insert (av, 4, release_status);
  array_insert (av, 5, MACHTYPE);

  VSETATTR (vv, att_readonly);
}
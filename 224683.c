sv_compare (sv1, sv2)
     struct name_and_function *sv1, *sv2;
{
  int r;

  if ((r = sv1->name[0] - sv2->name[0]) == 0)
    r = strcmp (sv1->name, sv2->name);
  return r;
}
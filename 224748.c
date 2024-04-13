sv_home (name)
     char *name;
{
  array_needs_making = 1;
  maybe_make_export_env ();
}
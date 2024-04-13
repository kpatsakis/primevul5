sv_globignore (name)
     char *name;
{
  if (privileged_mode == 0)
    setup_glob_ignore (name);
}
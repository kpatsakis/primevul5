get_random_number ()
{
  int rv, pid;

  /* Reset for command and process substitution. */
  pid = getpid ();
  if (subshell_environment && seeded_subshell != pid)
    {
      seedrand ();
      seeded_subshell = pid;
    }

  do
    rv = brand ();
  while (rv == last_random_value);
  return rv;
}
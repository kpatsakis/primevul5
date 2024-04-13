set_ppid ()
{
  char namebuf[INT_STRLEN_BOUND(pid_t) + 1], *name;
  SHELL_VAR *temp_var;

  name = inttostr (getppid (), namebuf, sizeof(namebuf));
  temp_var = find_variable ("PPID");
  if (temp_var)
    VUNSETATTR (temp_var, (att_readonly | att_exported));
  temp_var = bind_variable ("PPID", name, 0);
  VSETATTR (temp_var, (att_readonly | att_integer));
}
assign_subshell (var, value, unused, key)
     SHELL_VAR *var;
     char *value;
     arrayind_t unused;
     char *key;
{
  intmax_t new_value;

  if (value == 0 || *value == '\0' || legal_number (value, &new_value) == 0)
    new_value = 0;
  subshell_level = new_value;
  return var;
}
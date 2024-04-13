assign_seconds (self, value, unused, key)
     SHELL_VAR *self;
     char *value;
     arrayind_t unused;
     char *key;
{
  if (legal_number (value, &seconds_value_assigned) == 0)
    seconds_value_assigned = 0;
  shell_start_time = NOW;
  return (self);
}
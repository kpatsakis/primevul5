sv_history_control (name)
     char *name;
{
  char *temp;
  char *val;
  int tptr;

  history_control = 0;
  temp = get_string_value (name);

  if (temp == 0 || *temp == 0)
    return;

  tptr = 0;
  while (val = extract_colon_unit (temp, &tptr))
    {
      if (STREQ (val, "ignorespace"))
	history_control |= HC_IGNSPACE;
      else if (STREQ (val, "ignoredups"))
	history_control |= HC_IGNDUPS;
      else if (STREQ (val, "ignoreboth"))
	history_control |= HC_IGNBOTH;
      else if (STREQ (val, "erasedups"))
	history_control |= HC_ERASEDUPS;

      free (val);
    }
}
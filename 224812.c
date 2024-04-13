bind_function_def (name, value)
     const char *name;
     FUNCTION_DEF *value;
{
  FUNCTION_DEF *entry;
  BUCKET_CONTENTS *elt;
  COMMAND *cmd;

  entry = find_function_def (name);
  if (entry)
    {
      dispose_function_def_contents (entry);
      entry = copy_function_def_contents (value, entry);
    }
  else
    {
      cmd = value->command;
      value->command = 0;
      entry = copy_function_def (value);
      value->command = cmd;

      elt = hash_insert (savestring (name), shell_function_defs, HASH_NOSRCH);
      elt->data = (PTR_T *)entry;
    }
}
merge_temporary_env ()
{
  if (temporary_env)
    dispose_temporary_env (push_temp_var);
}
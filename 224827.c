set_shell_var ()
{
  SHELL_VAR *temp_var;

  temp_var = find_variable ("SHELL");
  if (temp_var == 0)
    {
      if (current_user.shell == 0)
	get_current_user_info ();
      temp_var = bind_variable ("SHELL", current_user.shell, 0);
    }
#if 0
  VSETATTR (temp_var, att_exported);
#endif
}
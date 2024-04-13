put_command_name_into_env (command_name)
     char *command_name;
{
  update_export_env_inplace ("_=", 2, command_name);
}
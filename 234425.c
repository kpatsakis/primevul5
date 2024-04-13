system_command()
{
    char *cmd;
    ++c_token;
    cmd = try_to_get_string();
    do_system(cmd);
    free(cmd);
}
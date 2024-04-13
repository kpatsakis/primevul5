static const char *get_ssh_command(void)
{
	const char *ssh;

	if ((ssh = getenv("GIT_SSH_COMMAND")))
		return ssh;

	if (!git_config_get_string_const("core.sshcommand", &ssh))
		return ssh;

	return NULL;
}
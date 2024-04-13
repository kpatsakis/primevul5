static int exec_grep(int argc, const char **argv)
{
	pid_t pid;
	int status;

	argv[argc] = NULL;
	pid = fork();
	if (pid < 0)
		return pid;
	if (!pid) {
		execvp("grep", (char **) argv);
		exit(255);
	}
	while (waitpid(pid, &status, 0) < 0) {
		if (errno == EINTR)
			continue;
		return -1;
	}
	if (WIFEXITED(status)) {
		if (!WEXITSTATUS(status))
			return 1;
		return 0;
	}
	return -1;
}
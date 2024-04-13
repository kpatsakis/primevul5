void omg_a_child_died(int /* sig */) {
	pid_t pid;
	int stat;
	while ((pid = waitpid(-1,&stat,WNOHANG)) > 0) { }
	::signal(SIGCHLD, omg_a_child_died); /* in case of unreliable signals */
}
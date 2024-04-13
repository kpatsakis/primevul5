checkoldpid(char* pidfile, int inchroot)
{
	pid_t old;
	if((old = readpid(pidfile)) != -1) {
		/* see if it is still alive */
		if(kill(old, 0) == 0 || errno == EPERM)
			log_warn("unbound is already running as pid %u.", 
				(unsigned)old);
		else	if(inchroot)
			log_warn("did not exit gracefully last time (%u)", 
				(unsigned)old);
	}
}
readpid (const char* file)
{
	int fd;
	pid_t pid;
	char pidbuf[32];
	char* t;
	ssize_t l;

	if ((fd = open(file, O_RDONLY)) == -1) {
		if(errno != ENOENT)
			log_err("Could not read pidfile %s: %s",
				file, strerror(errno));
		return -1;
	}

	if (((l = read(fd, pidbuf, sizeof(pidbuf)))) == -1) {
		if(errno != ENOENT)
			log_err("Could not read pidfile %s: %s",
				file, strerror(errno));
		close(fd);
		return -1;
	}

	close(fd);

	/* Empty pidfile means no pidfile... */
	if (l == 0) {
		return -1;
	}

	pidbuf[sizeof(pidbuf)-1] = 0;
	pid = (pid_t)strtol(pidbuf, &t, 10);
	
	if (*t && *t != '\n') {
		return -1;
	}
	return pid;
}
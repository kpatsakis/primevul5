main(int argc, char **argv)
{
	int i, retval = 0, dflag = 0, kflag = 0;
	const char *target_user = NULL, *user = NULL, *tty = NULL;
	struct passwd *pwd;
	struct timeval tv;
	fd_set write_fds;
	char path[BUFLEN];
	struct stat st;

	/* Check that there's nothing funny going on with stdio. */
	if ((fstat(STDIN_FILENO, &st) == -1) ||
	    (fstat(STDOUT_FILENO, &st) == -1) ||
	    (fstat(STDERR_FILENO, &st) == -1)) {
		/* Appropriate the "no controlling tty" error code. */
		return 3;
	}

	/* Parse arguments. */
	while ((i = getopt(argc, argv, "dk")) != -1) {
		switch (i) {
			case 'd':
				dflag++;
				break;
			case 'k':
				kflag++;
				break;
			default:
				fprintf(stderr, USAGE, argv[0]);
				return 1;
				break;
		}
	}

	/* Bail if both -k and -d are given together. */
	if ((kflag + dflag) > 1) {
		fprintf(stderr, USAGE, argv[0]);
		return 1;
	}

	/* Check that we're setuid. */
	if (geteuid() != 0) {
		fprintf(stderr, "%s must be setuid root\n",
			argv[0]);
		retval = 2;
	}

	/* Check that we have a controlling tty. */
	tty = ttyname(STDIN_FILENO);
	if ((tty == NULL) || (strlen(tty) == 0)) {
		tty = ttyname(STDOUT_FILENO);
	}
	if ((tty == NULL) || (strlen(tty) == 0)) {
		tty = ttyname(STDERR_FILENO);
	}
	if ((tty == NULL) || (strlen(tty) == 0)) {
		tty = "unknown";
	}

	/* Get the name of the invoking (requesting) user. */
	pwd = getpwuid(getuid());
	if (pwd == NULL) {
		retval = 4;
	}

	/* Get the name of the target user. */
	user = strdup(pwd->pw_name);
	if (user == NULL) {
		retval = 4;
	} else {
		target_user = (optind < argc) ? argv[optind] : user;
		if ((strchr(target_user, '.') != NULL) ||
		    (strchr(target_user, '/') != NULL) ||
		    (strchr(target_user, '%') != NULL)) {
			fprintf(stderr, "unknown user: %s\n",
				target_user);
			retval = 4;
		}
	}

	/* Sanity check the tty to make sure we should be checking
	 * for timestamps which pertain to it. */
	if (retval == 0) {
		tty = check_tty(tty);
		if (tty == NULL) {
			fprintf(stderr, "invalid tty\n");
			retval = 6;
		}
	}

	do {
		/* Sanity check the timestamp directory itself. */
		if (retval == 0) {
			if (check_dir_perms(NULL, TIMESTAMPDIR) != PAM_SUCCESS) {
				retval = 5;
			}
		}

		if (retval == 0) {
			/* Generate the name of the timestamp file. */
			format_timestamp_name(path, sizeof(path), TIMESTAMPDIR,
					      tty, user, target_user);
		}

		if (retval == 0) {
			if (kflag) {
				/* Remove the timestamp. */
				if (lstat(path, &st) != -1) {
					retval = unlink(path);
				}
			} else {
				/* Check the timestamp. */
				if (lstat(path, &st) != -1) {
					/* Check oldest login against timestamp */
					if (check_login_time(user, st.st_mtime) != PAM_SUCCESS) {
						retval = 7;
					} else if (!timestamp_good(st.st_mtime, time(NULL),
							    DEFAULT_TIMESTAMP_TIMEOUT) == PAM_SUCCESS) {
						retval = 7;
					}
				} else {
					retval = 7;
				}
			}
		}

		if (dflag > 0) {
			struct timeval now;
			/* Send the would-be-returned value to our parent. */
			signal(SIGPIPE, SIG_DFL);
			fprintf(stdout, "%d\n", retval);
			fflush(stdout);
			/* Wait. */
			gettimeofday(&now, NULL);
			tv.tv_sec = CHECK_INTERVAL;
			/* round the sleep time to get woken up on a whole second */
			tv.tv_usec = 1000000 - now.tv_usec;
			if (now.tv_usec < 500000)
				tv.tv_sec--;
			FD_ZERO(&write_fds);
			FD_SET(STDOUT_FILENO, &write_fds);
			select(STDOUT_FILENO + 1,
			       NULL, NULL, &write_fds,
			       &tv);
			retval = 0;
		}
	} while (dflag > 0);

	return retval;
}
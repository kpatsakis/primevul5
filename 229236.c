pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv)
{
	struct stat st;
	time_t interval = DEFAULT_TIMESTAMP_TIMEOUT;
	int i, fd, debug = 0, verbose = 0;
	char path[BUFLEN], *p, *message, *message_end;
	long tmp;
	const void *void_service;
	const char *service;
	time_t now, then;

	/* Parse arguments. */
	for (i = 0; i < argc; i++) {
		if (strcmp(argv[i], "debug") == 0) {
			debug = 1;
		}
	}
	for (i = 0; i < argc; i++) {
		if (strncmp(argv[i], "timestamp_timeout=", 18) == 0) {
			tmp = strtol(argv[i] + 18, &p, 0);
			if ((p != NULL) && (*p == '\0')) {
				interval = tmp;
				if (debug) {
					pam_syslog(pamh, LOG_DEBUG,
					       "setting timeout to %ld"
					       " seconds", (long)interval);
				}
			}
		} else
		if (strcmp(argv[i], "verbose") == 0) {
			verbose = 1;
			if (debug) {
				pam_syslog(pamh, LOG_DEBUG,
				       "becoming more verbose");
			}
		}
	}

	if (flags & PAM_SILENT) {
		verbose = 0;
	}

	/* Get the name of the timestamp file. */
	if (get_timestamp_name(pamh, argc, argv,
			       path, sizeof(path)) != PAM_SUCCESS) {
		return PAM_AUTH_ERR;
	}

	/* Get the name of the service. */
	if (pam_get_item(pamh, PAM_SERVICE, &void_service) != PAM_SUCCESS) {
		service = NULL;
	} else {
		service = void_service;
	}
	if ((service == NULL) || (strlen(service) == 0)) {
		service = "(unknown)";
	}

	/* Open the timestamp file. */
	fd = open(path, O_RDONLY | O_NOFOLLOW);
	if (fd == -1) {
		if (debug) {
			pam_syslog(pamh, LOG_DEBUG,
			       "cannot open timestamp `%s': %m",
			       path);
		}
		return PAM_AUTH_ERR;
	}

	if (fstat(fd, &st) == 0) {
		int count;
		void *mac;
		size_t maclen;
		char ruser[BUFLEN];

		/* Check that the file is owned by the superuser. */
		if ((st.st_uid != 0) || (st.st_gid != 0)) {
			pam_syslog(pamh, LOG_ERR, "timestamp file `%s' is "
			       "not owned by root", path);
			close(fd);
			return PAM_AUTH_ERR;
		}

		/* Check that the file is a normal file. */
		if (!(S_ISREG(st.st_mode))) {
			pam_syslog(pamh, LOG_ERR, "timestamp file `%s' is "
			       "not a regular file", path);
			close(fd);
			return PAM_AUTH_ERR;
		}

		/* Check that the file is the expected size. */
		if (st.st_size == 0) {
			/* Invalid, but may have been created by sudo. */
			close(fd);
			return PAM_AUTH_ERR;
		}
		if (st.st_size !=
		    (off_t)(strlen(path) + 1 + sizeof(then) + hmac_sha1_size())) {
			pam_syslog(pamh, LOG_NOTICE, "timestamp file `%s' "
			       "appears to be corrupted", path);
			close(fd);
			return PAM_AUTH_ERR;
		}

		/* Read the file contents. */
		message = malloc(st.st_size);
		count = 0;
                if (!message) {
			close(fd);
			return PAM_BUF_ERR;
		}
		while (count < st.st_size) {
			i = read(fd, message + count, st.st_size - count);
			if ((i == 0) || (i == -1)) {
				break;
			}
			count += i;
		}
		if (count < st.st_size) {
			pam_syslog(pamh, LOG_NOTICE, "error reading timestamp "
				"file `%s': %m", path);
			close(fd);
			free(message);
			return PAM_AUTH_ERR;
		}
		message_end = message + strlen(path) + 1 + sizeof(then);

		/* Regenerate the MAC. */
		hmac_sha1_generate_file(pamh, &mac, &maclen, TIMESTAMPKEY, 0, 0,
					message, message_end - message);
		if ((mac == NULL) ||
		    (memcmp(path, message, strlen(path)) != 0) ||
		    (memcmp(mac, message_end, maclen) != 0)) {
			pam_syslog(pamh, LOG_NOTICE, "timestamp file `%s' is "
				"corrupted", path);
			close(fd);
			free(mac);
			free(message);
			return PAM_AUTH_ERR;
		}
		free(mac);
		memmove(&then, message + strlen(path) + 1, sizeof(then));
		free(message);

		/* Check oldest login against timestamp */
		if (get_ruser(pamh, ruser, sizeof(ruser)))
		{
			close(fd);
			return PAM_AUTH_ERR;
		}
		if (check_login_time(ruser, then) != PAM_SUCCESS)
		{
			pam_syslog(pamh, LOG_NOTICE, "timestamp file `%s' is "
			       "older than oldest login, disallowing "
			       "access to %s for user %s",
			       path, service, ruser);
			close(fd);
			return PAM_AUTH_ERR;
		}

		/* Compare the dates. */
		now = time(NULL);
		if (timestamp_good(then, now, interval) == PAM_SUCCESS) {
			close(fd);
			pam_syslog(pamh, LOG_NOTICE, "timestamp file `%s' is "
			       "only %ld seconds old, allowing access to %s "
			       "for user %s", path, (long) (now - st.st_mtime),
			       service, ruser);
			if (verbose) {
				verbose_success(pamh, now - st.st_mtime);
			}
			return PAM_SUCCESS;
		} else {
			close(fd);
			pam_syslog(pamh, LOG_NOTICE, "timestamp file `%s' has "
			       "unacceptable age (%ld seconds), disallowing "
			       "access to %s for user %s",
			       path, (long) (now - st.st_mtime),
			       service, ruser);
			return PAM_AUTH_ERR;
		}
	}
	close(fd);

	/* Fail by default. */
	return PAM_AUTH_ERR;
}
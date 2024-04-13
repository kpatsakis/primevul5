pam_sm_open_session(pam_handle_t *pamh, int flags UNUSED, int argc, const char **argv)
{
	char path[BUFLEN], subdir[BUFLEN], *text, *p;
	void *mac;
	size_t maclen;
	time_t now;
	int fd, i, debug = 0;

	/* Parse arguments. */
	for (i = 0; i < argc; i++) {
		if (strcmp(argv[i], "debug") == 0) {
			debug = 1;
		}
	}

	/* Get the name of the timestamp file. */
	if (get_timestamp_name(pamh, argc, argv,
			       path, sizeof(path)) != PAM_SUCCESS) {
		return PAM_SESSION_ERR;
	}

	/* Create the directory for the timestamp file if it doesn't already
	 * exist. */
	for (i = 1; path[i] != '\0'; i++) {
		if (path[i] == '/') {
			/* Attempt to create the directory. */
			strncpy(subdir, path, i);
			subdir[i] = '\0';
			if (mkdir(subdir, 0700) == 0) {
				/* Attempt to set the owner to the superuser. */
			        if (lchown(subdir, 0, 0) != 0) {
					if (debug) {
						pam_syslog(pamh, LOG_DEBUG,
						    "error setting permissions on `%s': %m",
						    subdir);
					}
					return PAM_SESSION_ERR;
				}
			} else {
				if (errno != EEXIST) {
					if (debug) {
						pam_syslog(pamh, LOG_DEBUG,
						    "error creating directory `%s': %m",
						    subdir);
					}
					return PAM_SESSION_ERR;
				}
			}
		}
	}

	/* Generate the message. */
	text = malloc(strlen(path) + 1 + sizeof(now) + hmac_sha1_size());
	if (text == NULL) {
		pam_syslog(pamh, LOG_ERR, "unable to allocate memory: %m");
		return PAM_SESSION_ERR;
	}
	p = text;

	strcpy(text, path);
	p += strlen(path) + 1;

	now = time(NULL);
	memmove(p, &now, sizeof(now));
	p += sizeof(now);

	/* Generate the MAC and append it to the plaintext. */
	hmac_sha1_generate_file(pamh, &mac, &maclen,
				TIMESTAMPKEY,
				0, 0,
				text, p - text);
	if (mac == NULL) {
		pam_syslog(pamh, LOG_ERR, "failure generating MAC: %m");
		free(text);
		return PAM_SESSION_ERR;
	}
	memmove(p, mac, maclen);
	p += maclen;
	free(mac);

	/* Open the file. */
	fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (fd == -1) {
		pam_syslog(pamh, LOG_ERR, "unable to open `%s': %m", path);
		free(text);
		return PAM_SESSION_ERR;
	}

	/* Attempt to set the owner to the superuser. */
	if (fchown(fd, 0, 0) != 0) {
	  if (debug) {
	    pam_syslog(pamh, LOG_DEBUG,
		       "error setting ownership of `%s': %m",
		       path);
	  }
	  close(fd);
	  free(text);
	  return PAM_SESSION_ERR;
	}


	/* Write the timestamp to the file. */
	if (write(fd, text, p - text) != p - text) {
		pam_syslog(pamh, LOG_ERR, "unable to write to `%s': %m", path);
		close(fd);
		free(text);
		return PAM_SESSION_ERR;
	}

	/* Close the file and return successfully. */
	close(fd);
	free(text);
	pam_syslog(pamh, LOG_DEBUG, "updated timestamp file `%s'", path);
	return PAM_SUCCESS;
}
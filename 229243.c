get_timestamp_name(pam_handle_t *pamh, int argc, const char **argv,
		   char *path, size_t len)
{
	const char *user, *tty;
	const void *void_tty;
	const char *tdir = TIMESTAMPDIR;
	char ruser[BUFLEN];
	int i, debug = 0;

	/* Parse arguments. */
	for (i = 0; i < argc; i++) {
		if (strcmp(argv[i], "debug") == 0) {
			debug = 1;
		}
	}
	for (i = 0; i < argc; i++) {
		if (strncmp(argv[i], "timestampdir=", 13) == 0) {
			tdir = argv[i] + 13;
			if (debug) {
				pam_syslog(pamh, LOG_DEBUG,
				       "storing timestamps in `%s'",
				       tdir);
			}
		}
	}
	i = check_dir_perms(pamh, tdir);
	if (i != PAM_SUCCESS) {
		return i;
	}
	/* Get the name of the target user. */
	if (pam_get_user(pamh, &user, NULL) != PAM_SUCCESS) {
		user = NULL;
	}
	if ((user == NULL) || (strlen(user) == 0)) {
		return PAM_AUTH_ERR;
	}
	if (debug) {
		pam_syslog(pamh, LOG_DEBUG, "becoming user `%s'", user);
	}
	/* Get the name of the source user. */
	if (get_ruser(pamh, ruser, sizeof(ruser)) || strlen(ruser) == 0) {
		return PAM_AUTH_ERR;
	}
	if (debug) {
		pam_syslog(pamh, LOG_DEBUG, "currently user `%s'", ruser);
	}
	/* Get the name of the terminal. */
	if (pam_get_item(pamh, PAM_TTY, &void_tty) != PAM_SUCCESS) {
		tty = NULL;
	} else {
		tty = void_tty;
	}
	if ((tty == NULL) || (strlen(tty) == 0)) {
		tty = ttyname(STDIN_FILENO);
		if ((tty == NULL) || (strlen(tty) == 0)) {
			tty = ttyname(STDOUT_FILENO);
		}
		if ((tty == NULL) || (strlen(tty) == 0)) {
			tty = ttyname(STDERR_FILENO);
		}
		if ((tty == NULL) || (strlen(tty) == 0)) {
			/* Match sudo's behavior for this case. */
			tty = "unknown";
		}
	}
	if (debug) {
		pam_syslog(pamh, LOG_DEBUG, "tty is `%s'", tty);
	}
	/* Snip off all but the last part of the tty name. */
	tty = check_tty(tty);
	if (tty == NULL) {
		return PAM_AUTH_ERR;
	}
	/* Generate the name of the file used to cache auth results.  These
	 * paths should jive with sudo's per-tty naming scheme. */
	if (format_timestamp_name(path, len, tdir, tty, ruser, user) >= (int)len) {
		return PAM_AUTH_ERR;
	}
	if (debug) {
		pam_syslog(pamh, LOG_DEBUG, "using timestamp file `%s'", path);
	}
	return PAM_SUCCESS;
}
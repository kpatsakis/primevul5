check_dir_perms(pam_handle_t *pamh, const char *tdir)
{
	char scratch[BUFLEN];
	struct stat st;
	int i;
	/* Check that the directory is "safe". */
	if ((tdir == NULL) || (strlen(tdir) == 0)) {
		return PAM_AUTH_ERR;
	}
	/* Iterate over the path, checking intermediate directories. */
	memset(scratch, 0, sizeof(scratch));
	for (i = 0; (tdir[i] != '\0') && (i < (int)sizeof(scratch)); i++) {
		scratch[i] = tdir[i];
		if ((scratch[i] == '/') || (tdir[i + 1] == '\0')) {
			/* We now have the name of a directory in the path, so
			 * we need to check it. */
			if ((lstat(scratch, &st) == -1) && (errno != ENOENT)) {
				pam_syslog(pamh, LOG_ERR,
				       "unable to read `%s': %m",
				       scratch);
				return PAM_AUTH_ERR;
			}
			if (!S_ISDIR(st.st_mode)) {
				pam_syslog(pamh, LOG_ERR,
				       "`%s' is not a directory",
				       scratch);
				return PAM_AUTH_ERR;
			}
			if (S_ISLNK(st.st_mode)) {
				pam_syslog(pamh, LOG_ERR,
				       "`%s' is a symbolic link",
				       scratch);
				return PAM_AUTH_ERR;
			}
			if (st.st_uid != 0) {
				pam_syslog(pamh, LOG_ERR,
				       "`%s' owner UID != 0",
				       scratch);
				return PAM_AUTH_ERR;
			}
			if (st.st_gid != 0) {
				pam_syslog(pamh, LOG_ERR,
				       "`%s' owner GID != 0",
				       scratch);
				return PAM_AUTH_ERR;
			}
			if ((st.st_mode & (S_IWGRP | S_IWOTH)) != 0) {
				pam_syslog(pamh, LOG_ERR,
				       "`%s' permissions are lax",
				       scratch);
				return PAM_AUTH_ERR;
			}
		}
	}
	return PAM_SUCCESS;
}
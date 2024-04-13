static int override_ssh_variant(int *port_option, int *needs_batch)
{
	char *variant;

	variant = xstrdup_or_null(getenv("GIT_SSH_VARIANT"));
	if (!variant &&
	    git_config_get_string("ssh.variant", &variant))
		return 0;

	if (!strcmp(variant, "plink") || !strcmp(variant, "putty")) {
		*port_option = 'P';
		*needs_batch = 0;
	} else if (!strcmp(variant, "tortoiseplink")) {
		*port_option = 'P';
		*needs_batch = 1;
	} else {
		*port_option = 'p';
		*needs_batch = 0;
	}
	free(variant);
	return 1;
}
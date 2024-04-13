static void handle_ssh_variant(const char *ssh_command, int is_cmdline,
			       int *port_option, int *needs_batch)
{
	const char *variant;
	char *p = NULL;

	if (override_ssh_variant(port_option, needs_batch))
		return;

	if (!is_cmdline) {
		p = xstrdup(ssh_command);
		variant = basename(p);
	} else {
		const char **ssh_argv;

		p = xstrdup(ssh_command);
		if (split_cmdline(p, &ssh_argv) > 0) {
			variant = basename((char *)ssh_argv[0]);
			/*
			 * At this point, variant points into the buffer
			 * referenced by p, hence we do not need ssh_argv
			 * any longer.
			 */
			free(ssh_argv);
		} else {
			free(p);
			return;
		}
	}

	if (!strcasecmp(variant, "plink") ||
	    !strcasecmp(variant, "plink.exe"))
		*port_option = 'P';
	else if (!strcasecmp(variant, "tortoiseplink") ||
		 !strcasecmp(variant, "tortoiseplink.exe")) {
		*port_option = 'P';
		*needs_batch = 1;
	}
	free(p);
}
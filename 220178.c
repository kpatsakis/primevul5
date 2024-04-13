extern int x11_set_xauth(char *xauthority, char *cookie,
			 char *host, uint16_t display)
{
	int i=0, status;
	char *result;
	char **xauth_argv;
	char template[] = "/tmp/xauth-source-XXXXXX";
	char *contents = NULL;
	int fd;

	/* protect against weak file permissions in old glibc */
	umask(0077);
	if ((fd = mkstemp(template)) < 0)
		fatal("%s: could not create temp file", __func__);

	xstrfmtcat(contents, "add %s/unix:%u MIT-MAGIC-COOKIE-1 %s\n",
		   host, display, cookie);
	safe_write(fd, contents, strlen(contents));
	xfree(contents);
	close(fd);

	xauth_argv = xmalloc(sizeof(char *) * 10);
	xauth_argv[i++] = "xauth";
	xauth_argv[i++] = "-v";
	xauth_argv[i++] = "-f";
	xauth_argv[i++] = xauthority;
	xauth_argv[i++] = "source";
	xauth_argv[i++] = template;
	xauth_argv[i++] = NULL;
	xassert(i < 10);

	result = run_command("xauth", XAUTH_PATH, xauth_argv, 10000, 0,
			     &status);

	(void) unlink(template);
	xfree(xauth_argv);

	debug2("%s: result from xauth: %s", __func__, result);
	xfree(result);

	return status;

rwfail:
	fatal("%s: could not write temporary xauth file", __func__);
	return SLURM_ERROR;
}
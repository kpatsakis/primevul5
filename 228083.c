static int exec_helper(struct libmnt_context *cxt)
{
	char *namespace = NULL;
	struct libmnt_ns *ns_tgt = mnt_context_get_target_ns(cxt);
	int rc;
	pid_t pid;

	assert(cxt);
	assert(cxt->fs);
	assert(cxt->helper);
	assert((cxt->flags & MNT_FL_MOUNTFLAGS_MERGED));
	assert(cxt->helper_exec_status == 1);

	if (mnt_context_is_fake(cxt)) {
		DBG(CXT, ul_debugobj(cxt, "fake mode: does not execute helper"));
		cxt->helper_exec_status = rc = 0;
		return rc;
	}

	if (ns_tgt->fd != -1
	    && asprintf(&namespace, "/proc/%i/fd/%i",
			getpid(), ns_tgt->fd) == -1) {
		return -ENOMEM;
	}

	DBG_FLUSH;

	pid = fork();
	switch (pid) {
	case 0:
	{
		const char *args[12], *type;
		int i = 0;

		if (drop_permissions() != 0)
			_exit(EXIT_FAILURE);

		if (!mnt_context_switch_origin_ns(cxt))
			_exit(EXIT_FAILURE);

		type = mnt_fs_get_fstype(cxt->fs);

		args[i++] = cxt->helper;			/* 1 */
		args[i++] = mnt_fs_get_target(cxt->fs);		/* 2 */

		if (mnt_context_is_nomtab(cxt))
			args[i++] = "-n";			/* 3 */
		if (mnt_context_is_lazy(cxt))
			args[i++] = "-l";			/* 4 */
		if (mnt_context_is_force(cxt))
			args[i++] = "-f";			/* 5 */
		if (mnt_context_is_verbose(cxt))
			args[i++] = "-v";			/* 6 */
		if (mnt_context_is_rdonly_umount(cxt))
			args[i++] = "-r";			/* 7 */
		if (type
		    && strchr(type, '.')
		    && !endswith(cxt->helper, type)) {
			args[i++] = "-t";			/* 8 */
			args[i++] = type;			/* 9 */
		}
		if (namespace) {
			args[i++] = "-N";			/* 10 */
			args[i++] = namespace;			/* 11 */
		}

		args[i] = NULL;					/* 12 */
		for (i = 0; args[i]; i++)
			DBG(CXT, ul_debugobj(cxt, "argv[%d] = \"%s\"",
							i, args[i]));
		DBG_FLUSH;
		execv(cxt->helper, (char * const *) args);
		_exit(EXIT_FAILURE);
	}
	default:
	{
		int st;

		if (waitpid(pid, &st, 0) == (pid_t) -1) {
			cxt->helper_status = -1;
			rc = -errno;
		} else {
			cxt->helper_status = WIFEXITED(st) ? WEXITSTATUS(st) : -1;
			cxt->helper_exec_status = rc = 0;
		}
		DBG(CXT, ul_debugobj(cxt, "%s executed [status=%d, rc=%d%s]",
				cxt->helper,
				cxt->helper_status, rc,
				rc ? " waitpid failed" : ""));
		break;
	}

	case -1:
		cxt->helper_exec_status = rc = -errno;
		DBG(CXT, ul_debugobj(cxt, "fork() failed"));
		break;
	}

	free(namespace);
	return rc;
}
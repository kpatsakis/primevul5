int fpm_unix_resolve_socket_premissions(struct fpm_worker_pool_s *wp) /* {{{ */
{
	struct fpm_worker_pool_config_s *c = wp->config;

	/* uninitialized */
	wp->socket_uid = -1;
	wp->socket_gid = -1;
	wp->socket_mode = 0666;

	if (!c) {
		return 0;
	}

	if (c->listen_owner && *c->listen_owner) {
		struct passwd *pwd;

		pwd = getpwnam(c->listen_owner);
		if (!pwd) {
			zlog(ZLOG_SYSERROR, "[pool %s] cannot get uid for user '%s'", wp->config->name, c->listen_owner);
			return -1;
		}

		wp->socket_uid = pwd->pw_uid;
		wp->socket_gid = pwd->pw_gid;
	}

	if (c->listen_group && *c->listen_group) {
		struct group *grp;

		grp = getgrnam(c->listen_group);
		if (!grp) {
			zlog(ZLOG_SYSERROR, "[pool %s] cannot get gid for group '%s'", wp->config->name, c->listen_group);
			return -1;
		}
		wp->socket_gid = grp->gr_gid;
	}

	if (c->listen_mode && *c->listen_mode) {
		wp->socket_mode = strtoul(c->listen_mode, 0, 8);
	}
	return 0;
}
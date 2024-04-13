static void process_init_reply(struct fuse_mount *fm, struct fuse_args *args,
			       int error)
{
	struct fuse_conn *fc = fm->fc;
	struct fuse_init_args *ia = container_of(args, typeof(*ia), args);
	struct fuse_init_out *arg = &ia->out;
	bool ok = true;

	if (error || arg->major != FUSE_KERNEL_VERSION)
		ok = false;
	else {
		unsigned long ra_pages;

		process_init_limits(fc, arg);

		if (arg->minor >= 6) {
			ra_pages = arg->max_readahead / PAGE_SIZE;
			if (arg->flags & FUSE_ASYNC_READ)
				fc->async_read = 1;
			if (!(arg->flags & FUSE_POSIX_LOCKS))
				fc->no_lock = 1;
			if (arg->minor >= 17) {
				if (!(arg->flags & FUSE_FLOCK_LOCKS))
					fc->no_flock = 1;
			} else {
				if (!(arg->flags & FUSE_POSIX_LOCKS))
					fc->no_flock = 1;
			}
			if (arg->flags & FUSE_ATOMIC_O_TRUNC)
				fc->atomic_o_trunc = 1;
			if (arg->minor >= 9) {
				/* LOOKUP has dependency on proto version */
				if (arg->flags & FUSE_EXPORT_SUPPORT)
					fc->export_support = 1;
			}
			if (arg->flags & FUSE_BIG_WRITES)
				fc->big_writes = 1;
			if (arg->flags & FUSE_DONT_MASK)
				fc->dont_mask = 1;
			if (arg->flags & FUSE_AUTO_INVAL_DATA)
				fc->auto_inval_data = 1;
			else if (arg->flags & FUSE_EXPLICIT_INVAL_DATA)
				fc->explicit_inval_data = 1;
			if (arg->flags & FUSE_DO_READDIRPLUS) {
				fc->do_readdirplus = 1;
				if (arg->flags & FUSE_READDIRPLUS_AUTO)
					fc->readdirplus_auto = 1;
			}
			if (arg->flags & FUSE_ASYNC_DIO)
				fc->async_dio = 1;
			if (arg->flags & FUSE_WRITEBACK_CACHE)
				fc->writeback_cache = 1;
			if (arg->flags & FUSE_PARALLEL_DIROPS)
				fc->parallel_dirops = 1;
			if (arg->flags & FUSE_HANDLE_KILLPRIV)
				fc->handle_killpriv = 1;
			if (arg->time_gran && arg->time_gran <= 1000000000)
				fm->sb->s_time_gran = arg->time_gran;
			if ((arg->flags & FUSE_POSIX_ACL)) {
				fc->default_permissions = 1;
				fc->posix_acl = 1;
				fm->sb->s_xattr = fuse_acl_xattr_handlers;
			}
			if (arg->flags & FUSE_CACHE_SYMLINKS)
				fc->cache_symlinks = 1;
			if (arg->flags & FUSE_ABORT_ERROR)
				fc->abort_err = 1;
			if (arg->flags & FUSE_MAX_PAGES) {
				fc->max_pages =
					min_t(unsigned int, FUSE_MAX_MAX_PAGES,
					max_t(unsigned int, arg->max_pages, 1));
			}
			if (IS_ENABLED(CONFIG_FUSE_DAX) &&
			    arg->flags & FUSE_MAP_ALIGNMENT &&
			    !fuse_dax_check_alignment(fc, arg->map_alignment)) {
				ok = false;
			}
			if (arg->flags & FUSE_HANDLE_KILLPRIV_V2) {
				fc->handle_killpriv_v2 = 1;
				fm->sb->s_flags |= SB_NOSEC;
			}
		} else {
			ra_pages = fc->max_read / PAGE_SIZE;
			fc->no_lock = 1;
			fc->no_flock = 1;
		}

		fm->sb->s_bdi->ra_pages =
				min(fm->sb->s_bdi->ra_pages, ra_pages);
		fc->minor = arg->minor;
		fc->max_write = arg->minor < 5 ? 4096 : arg->max_write;
		fc->max_write = max_t(unsigned, 4096, fc->max_write);
		fc->conn_init = 1;
	}
	kfree(ia);

	if (!ok) {
		fc->conn_init = 0;
		fc->conn_error = 1;
	}

	fuse_set_initialized(fc);
	wake_up_all(&fc->blocked_waitq);
}
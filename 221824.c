static void fuse_file_put(struct fuse_file *ff, bool sync, bool isdir)
{
	if (refcount_dec_and_test(&ff->count)) {
		struct fuse_args *args = &ff->release_args->args;

		if (isdir ? ff->fm->fc->no_opendir : ff->fm->fc->no_open) {
			/* Do nothing when client does not implement 'open' */
			fuse_release_end(ff->fm, args, 0);
		} else if (sync) {
			fuse_simple_request(ff->fm, args);
			fuse_release_end(ff->fm, args, 0);
		} else {
			args->end = fuse_release_end;
			if (fuse_simple_background(ff->fm, args,
						   GFP_KERNEL | __GFP_NOFAIL))
				fuse_release_end(ff->fm, args, -ENOTCONN);
		}
		kfree(ff);
	}
}
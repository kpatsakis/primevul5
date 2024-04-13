static bool fuse_emit(struct file *file, struct dir_context *ctx,
		      struct fuse_dirent *dirent)
{
	struct fuse_file *ff = file->private_data;

	if (ff->open_flags & FOPEN_CACHE_DIR)
		fuse_add_dirent_to_cache(file, dirent, ctx->pos);

	return dir_emit(ctx, dirent->name, dirent->namelen, dirent->ino,
			dirent->type);
}
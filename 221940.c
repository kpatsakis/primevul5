static enum fuse_parse_result fuse_parse_cache(struct fuse_file *ff,
					       void *addr, unsigned int size,
					       struct dir_context *ctx)
{
	unsigned int offset = ff->readdir.cache_off & ~PAGE_MASK;
	enum fuse_parse_result res = FOUND_NONE;

	WARN_ON(offset >= size);

	for (;;) {
		struct fuse_dirent *dirent = addr + offset;
		unsigned int nbytes = size - offset;
		size_t reclen;

		if (nbytes < FUSE_NAME_OFFSET || !dirent->namelen)
			break;

		reclen = FUSE_DIRENT_SIZE(dirent); /* derefs ->namelen */

		if (WARN_ON(dirent->namelen > FUSE_NAME_MAX))
			return FOUND_ERR;
		if (WARN_ON(reclen > nbytes))
			return FOUND_ERR;
		if (WARN_ON(memchr(dirent->name, '/', dirent->namelen) != NULL))
			return FOUND_ERR;

		if (ff->readdir.pos == ctx->pos) {
			res = FOUND_SOME;
			if (!dir_emit(ctx, dirent->name, dirent->namelen,
				      dirent->ino, dirent->type))
				return FOUND_ALL;
			ctx->pos = dirent->off;
		}
		ff->readdir.pos = dirent->off;
		ff->readdir.cache_off += reclen;

		offset += reclen;
	}

	return res;
}
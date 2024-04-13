static int nfs_iterate(struct file *file, struct dir_context *ctx)
{
	struct dentry *dentry = file->f_path.dentry;
	struct inode *dir = d_inode(dentry);
	struct nfs_priv *npriv = nfsi(dir)->npriv;
	void *buf = NULL;
	struct nfs_dir *ndir;
	struct xdr_stream *xdr;
	int ret;
	uint32_t *p, len;

	ndir = xzalloc(sizeof(*ndir));
	ndir->fh = nfsi(dir)->fh;

	while (1) {
		/* cookie == 0 and cookieverf == 0 means start of dir */
		buf = nfs_readdirattr_req(npriv, ndir);
		if (!buf) {
			pr_err("%s: nfs_readdirattr_req failed\n", __func__);
			ret = -EINVAL;
			goto out;
		}

		xdr = &ndir->stream;

		while (1) {
			char name[256];

			p = xdr_inline_decode(xdr, 4);
			if (!p)
				goto err_eop;

			if (!net_read_uint32(p)) {
				/* eof? */
				p = xdr_inline_decode(xdr, 4);
				if (!p)
					goto err_eop;

				if (net_read_uint32(p)) {
					ret = 0;
					goto out;
				}

				break;
			}

			/* skip over fileid */
			p = xdr_inline_decode(xdr, 8);
			if (!p)
				goto err_eop;

			ret = decode_filename(xdr, name, &len);
			if (ret)
				goto out;

			dir_emit(ctx, name, len, 0, DT_UNKNOWN);

			p = xdr_inline_decode(xdr, 8);
			if (!p)
				goto err_eop;

			ndir->cookie = ntoh64(net_read_uint64(p));
		}
		free(buf);
	}

	ret = 0;

out:
	free(ndir->stream.buf);
	free(ndir);

	return ret;

err_eop:
	pr_err("Unexpected end of packet\n");

	return -EIO;
}

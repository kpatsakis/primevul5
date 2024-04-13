static int decode_pathname(struct xdr_stream *xdr, struct nfs4_pathname *path)
{
	u32 n;
	__be32 *p;
	int status = 0;

	p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p))
		return -EIO;
	n = be32_to_cpup(p);
	if (n == 0)
		goto root_path;
	dprintk("pathname4: ");
	if (n > NFS4_PATHNAME_MAXCOMPONENTS) {
		dprintk("cannot parse %d components in path\n", n);
		goto out_eio;
	}
	for (path->ncomponents = 0; path->ncomponents < n; path->ncomponents++) {
		struct nfs4_string *component = &path->components[path->ncomponents];
		status = decode_opaque_inline(xdr, &component->len, &component->data);
		if (unlikely(status != 0))
			goto out_eio;
		ifdebug (XDR)
			pr_cont("%s%.*s ",
				(path->ncomponents != n ? "/ " : ""),
				component->len, component->data);
	}
out:
	return status;
root_path:
/* a root pathname is sent as a zero component4 */
	path->ncomponents = 1;
	path->components[0].len=0;
	path->components[0].data=NULL;
	dprintk("pathname4: /\n");
	goto out;
out_eio:
	dprintk(" status %d", status);
	status = -EIO;
	goto out;
}
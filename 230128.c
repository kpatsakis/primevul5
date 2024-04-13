static struct nfs4_createdata *nfs4_alloc_createdata(struct inode *dir,
		const struct qstr *name, struct iattr *sattr, u32 ftype)
{
	struct nfs4_createdata *data;

	data = kzalloc(sizeof(*data), GFP_KERNEL);
	if (data != NULL) {
		struct nfs_server *server = NFS_SERVER(dir);

		data->label = nfs4_label_alloc(server, GFP_KERNEL);
		if (IS_ERR(data->label))
			goto out_free;

		data->msg.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_CREATE];
		data->msg.rpc_argp = &data->arg;
		data->msg.rpc_resp = &data->res;
		data->arg.dir_fh = NFS_FH(dir);
		data->arg.server = server;
		data->arg.name = name;
		data->arg.attrs = sattr;
		data->arg.ftype = ftype;
		data->arg.bitmask = nfs4_bitmask(server, data->label);
		data->arg.umask = current_umask();
		data->res.server = server;
		data->res.fh = &data->fh;
		data->res.fattr = &data->fattr;
		data->res.label = data->label;
		nfs_fattr_init(data->res.fattr);
	}
	return data;
out_free:
	kfree(data);
	return NULL;
}
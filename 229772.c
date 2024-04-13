int nfs4_proc_get_rootfh(struct nfs_server *server, struct nfs_fh *fhandle,
			 struct nfs_fsinfo *info,
			 bool auth_probe)
{
	int status = 0;

	if (!auth_probe)
		status = nfs4_lookup_root(server, fhandle, info);

	if (auth_probe || status == NFS4ERR_WRONGSEC)
		status = server->nfs_client->cl_mvops->find_root_sec(server,
				fhandle, info);

	if (status == 0)
		status = nfs4_server_capabilities(server, fhandle);
	if (status == 0)
		status = nfs4_do_fsinfo(server, fhandle, info);

	return nfs4_map_errors(status);
}
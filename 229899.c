nfs41_find_root_sec(struct nfs_server *server, struct nfs_fh *fhandle,
		    struct nfs_fsinfo *info)
{
	int err;
	struct page *page;
	rpc_authflavor_t flavor = RPC_AUTH_MAXFLAVOR;
	struct nfs4_secinfo_flavors *flavors;
	struct nfs4_secinfo4 *secinfo;
	int i;

	page = alloc_page(GFP_KERNEL);
	if (!page) {
		err = -ENOMEM;
		goto out;
	}

	flavors = page_address(page);
	err = nfs41_proc_secinfo_no_name(server, fhandle, info, flavors);

	/*
	 * Fall back on "guess and check" method if
	 * the server doesn't support SECINFO_NO_NAME
	 */
	if (err == -NFS4ERR_WRONGSEC || err == -ENOTSUPP) {
		err = nfs4_find_root_sec(server, fhandle, info);
		goto out_freepage;
	}
	if (err)
		goto out_freepage;

	for (i = 0; i < flavors->num_flavors; i++) {
		secinfo = &flavors->flavors[i];

		switch (secinfo->flavor) {
		case RPC_AUTH_NULL:
		case RPC_AUTH_UNIX:
		case RPC_AUTH_GSS:
			flavor = rpcauth_get_pseudoflavor(secinfo->flavor,
					&secinfo->flavor_info);
			break;
		default:
			flavor = RPC_AUTH_MAXFLAVOR;
			break;
		}

		if (!nfs_auth_info_match(&server->auth_info, flavor))
			flavor = RPC_AUTH_MAXFLAVOR;

		if (flavor != RPC_AUTH_MAXFLAVOR) {
			err = nfs4_lookup_root_sec(server, fhandle,
						   info, flavor);
			if (!err)
				break;
		}
	}

	if (flavor == RPC_AUTH_MAXFLAVOR)
		err = -EPERM;

out_freepage:
	put_page(page);
	if (err == -EACCES)
		return -EPERM;
out:
	return err;
}
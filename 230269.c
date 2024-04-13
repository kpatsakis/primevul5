static int _nfs4_server_capabilities(struct nfs_server *server, struct nfs_fh *fhandle)
{
	u32 bitmask[3] = {}, minorversion = server->nfs_client->cl_minorversion;
	struct nfs4_server_caps_arg args = {
		.fhandle = fhandle,
		.bitmask = bitmask,
	};
	struct nfs4_server_caps_res res = {};
	struct rpc_message msg = {
		.rpc_proc = &nfs4_procedures[NFSPROC4_CLNT_SERVER_CAPS],
		.rpc_argp = &args,
		.rpc_resp = &res,
	};
	int status;
	int i;

	bitmask[0] = FATTR4_WORD0_SUPPORTED_ATTRS |
		     FATTR4_WORD0_FH_EXPIRE_TYPE |
		     FATTR4_WORD0_LINK_SUPPORT |
		     FATTR4_WORD0_SYMLINK_SUPPORT |
		     FATTR4_WORD0_ACLSUPPORT;
	if (minorversion)
		bitmask[2] = FATTR4_WORD2_SUPPATTR_EXCLCREAT;

	status = nfs4_call_sync(server->client, server, &msg, &args.seq_args, &res.seq_res, 0);
	if (status == 0) {
		/* Sanity check the server answers */
		switch (minorversion) {
		case 0:
			res.attr_bitmask[1] &= FATTR4_WORD1_NFS40_MASK;
			res.attr_bitmask[2] = 0;
			break;
		case 1:
			res.attr_bitmask[2] &= FATTR4_WORD2_NFS41_MASK;
			break;
		case 2:
			res.attr_bitmask[2] &= FATTR4_WORD2_NFS42_MASK;
		}
		memcpy(server->attr_bitmask, res.attr_bitmask, sizeof(server->attr_bitmask));
		server->caps &= ~(NFS_CAP_ACLS|NFS_CAP_HARDLINKS|
				NFS_CAP_SYMLINKS|NFS_CAP_FILEID|
				NFS_CAP_MODE|NFS_CAP_NLINK|NFS_CAP_OWNER|
				NFS_CAP_OWNER_GROUP|NFS_CAP_ATIME|
				NFS_CAP_CTIME|NFS_CAP_MTIME|
				NFS_CAP_SECURITY_LABEL);
		if (res.attr_bitmask[0] & FATTR4_WORD0_ACL &&
				res.acl_bitmask & ACL4_SUPPORT_ALLOW_ACL)
			server->caps |= NFS_CAP_ACLS;
		if (res.has_links != 0)
			server->caps |= NFS_CAP_HARDLINKS;
		if (res.has_symlinks != 0)
			server->caps |= NFS_CAP_SYMLINKS;
		if (res.attr_bitmask[0] & FATTR4_WORD0_FILEID)
			server->caps |= NFS_CAP_FILEID;
		if (res.attr_bitmask[1] & FATTR4_WORD1_MODE)
			server->caps |= NFS_CAP_MODE;
		if (res.attr_bitmask[1] & FATTR4_WORD1_NUMLINKS)
			server->caps |= NFS_CAP_NLINK;
		if (res.attr_bitmask[1] & FATTR4_WORD1_OWNER)
			server->caps |= NFS_CAP_OWNER;
		if (res.attr_bitmask[1] & FATTR4_WORD1_OWNER_GROUP)
			server->caps |= NFS_CAP_OWNER_GROUP;
		if (res.attr_bitmask[1] & FATTR4_WORD1_TIME_ACCESS)
			server->caps |= NFS_CAP_ATIME;
		if (res.attr_bitmask[1] & FATTR4_WORD1_TIME_METADATA)
			server->caps |= NFS_CAP_CTIME;
		if (res.attr_bitmask[1] & FATTR4_WORD1_TIME_MODIFY)
			server->caps |= NFS_CAP_MTIME;
#ifdef CONFIG_NFS_V4_SECURITY_LABEL
		if (res.attr_bitmask[2] & FATTR4_WORD2_SECURITY_LABEL)
			server->caps |= NFS_CAP_SECURITY_LABEL;
#endif
		memcpy(server->attr_bitmask_nl, res.attr_bitmask,
				sizeof(server->attr_bitmask));
		server->attr_bitmask_nl[2] &= ~FATTR4_WORD2_SECURITY_LABEL;

		memcpy(server->cache_consistency_bitmask, res.attr_bitmask, sizeof(server->cache_consistency_bitmask));
		server->cache_consistency_bitmask[0] &= FATTR4_WORD0_CHANGE|FATTR4_WORD0_SIZE;
		server->cache_consistency_bitmask[1] &= FATTR4_WORD1_TIME_METADATA|FATTR4_WORD1_TIME_MODIFY;
		server->cache_consistency_bitmask[2] = 0;

		/* Avoid a regression due to buggy server */
		for (i = 0; i < ARRAY_SIZE(res.exclcreat_bitmask); i++)
			res.exclcreat_bitmask[i] &= res.attr_bitmask[i];
		memcpy(server->exclcreat_bitmask, res.exclcreat_bitmask,
			sizeof(server->exclcreat_bitmask));

		server->acl_bitmask = res.acl_bitmask;
		server->fh_expire_type = res.fh_expire_type;
	}

	return status;
}
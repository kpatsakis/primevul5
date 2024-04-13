nfs4_opendata_get_inode(struct nfs4_opendata *data)
{
	struct inode *inode;

	switch (data->o_arg.claim) {
	case NFS4_OPEN_CLAIM_NULL:
	case NFS4_OPEN_CLAIM_DELEGATE_CUR:
	case NFS4_OPEN_CLAIM_DELEGATE_PREV:
		if (!(data->f_attr.valid & NFS_ATTR_FATTR))
			return ERR_PTR(-EAGAIN);
		inode = nfs_fhget(data->dir->d_sb, &data->o_res.fh,
				&data->f_attr, data->f_label);
		break;
	default:
		inode = d_inode(data->dentry);
		ihold(inode);
		nfs_refresh_inode(inode, &data->f_attr);
	}
	return inode;
}
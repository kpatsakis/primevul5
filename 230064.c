static void nfs_fixup_secinfo_attributes(struct nfs_fattr *fattr)
{
	fattr->valid |= NFS_ATTR_FATTR_TYPE | NFS_ATTR_FATTR_MODE |
		NFS_ATTR_FATTR_NLINK | NFS_ATTR_FATTR_MOUNTPOINT;
	fattr->mode = S_IFDIR | S_IRUGO | S_IXUGO;
	fattr->nlink = 2;
}
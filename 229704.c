static bool nfs4_xattr_list_nfs4_acl(struct dentry *dentry)
{
	return nfs4_server_supports_acls(NFS_SERVER(d_inode(dentry)));
}
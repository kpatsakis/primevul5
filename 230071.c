static int decode_attr_group(struct xdr_stream *xdr, uint32_t *bitmap,
		const struct nfs_server *server, kgid_t *gid,
		struct nfs4_string *group_name)
{
	ssize_t len;
	char *p;

	*gid = make_kgid(&init_user_ns, -2);
	if (unlikely(bitmap[1] & (FATTR4_WORD1_OWNER_GROUP - 1U)))
		return -EIO;
	if (!(bitmap[1] & FATTR4_WORD1_OWNER_GROUP))
		return 0;
	bitmap[1] &= ~FATTR4_WORD1_OWNER_GROUP;

	if (group_name != NULL) {
		len = decode_nfs4_string(xdr, group_name, GFP_NOIO);
		if (len <= 0)
			goto out;
		dprintk("%s: name=%s\n", __func__, group_name->data);
		return NFS_ATTR_FATTR_GROUP_NAME;
	} else {
		len = xdr_stream_decode_opaque_inline(xdr, (void **)&p,
				XDR_MAX_NETOBJ);
		if (len <= 0 || nfs_map_group_to_gid(server, p, len, gid) != 0)
			goto out;
		dprintk("%s: gid=%d\n", __func__, (int)from_kgid(&init_user_ns, *gid));
		return NFS_ATTR_FATTR_GROUP;
	}
out:
	if (len == -EBADMSG)
		return -EIO;
	return 0;
}
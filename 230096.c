static int decode_attr_owner(struct xdr_stream *xdr, uint32_t *bitmap,
		const struct nfs_server *server, kuid_t *uid,
		struct nfs4_string *owner_name)
{
	ssize_t len;
	char *p;

	*uid = make_kuid(&init_user_ns, -2);
	if (unlikely(bitmap[1] & (FATTR4_WORD1_OWNER - 1U)))
		return -EIO;
	if (!(bitmap[1] & FATTR4_WORD1_OWNER))
		return 0;
	bitmap[1] &= ~FATTR4_WORD1_OWNER;

	if (owner_name != NULL) {
		len = decode_nfs4_string(xdr, owner_name, GFP_NOIO);
		if (len <= 0)
			goto out;
		dprintk("%s: name=%s\n", __func__, owner_name->data);
		return NFS_ATTR_FATTR_OWNER_NAME;
	} else {
		len = xdr_stream_decode_opaque_inline(xdr, (void **)&p,
				XDR_MAX_NETOBJ);
		if (len <= 0 || nfs_map_name_to_uid(server, p, len, uid) != 0)
			goto out;
		dprintk("%s: uid=%d\n", __func__, (int)from_kuid(&init_user_ns, *uid));
		return NFS_ATTR_FATTR_OWNER;
	}
out:
	if (len == -EBADMSG)
		return -EIO;
	return 0;
}
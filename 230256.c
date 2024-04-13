static void encode_attrs(struct xdr_stream *xdr, const struct iattr *iap,
				const struct nfs4_label *label,
				const umode_t *umask,
				const struct nfs_server *server,
				const uint32_t attrmask[])
{
	char owner_name[IDMAP_NAMESZ];
	char owner_group[IDMAP_NAMESZ];
	int owner_namelen = 0;
	int owner_grouplen = 0;
	__be32 *p;
	uint32_t len = 0;
	uint32_t bmval[3] = { 0 };

	/*
	 * We reserve enough space to write the entire attribute buffer at once.
	 */
	if ((iap->ia_valid & ATTR_SIZE) && (attrmask[0] & FATTR4_WORD0_SIZE)) {
		bmval[0] |= FATTR4_WORD0_SIZE;
		len += 8;
	}
	if (iap->ia_valid & ATTR_MODE) {
		if (umask && (attrmask[2] & FATTR4_WORD2_MODE_UMASK)) {
			bmval[2] |= FATTR4_WORD2_MODE_UMASK;
			len += 8;
		} else if (attrmask[1] & FATTR4_WORD1_MODE) {
			bmval[1] |= FATTR4_WORD1_MODE;
			len += 4;
		}
	}
	if ((iap->ia_valid & ATTR_UID) && (attrmask[1] & FATTR4_WORD1_OWNER)) {
		owner_namelen = nfs_map_uid_to_name(server, iap->ia_uid, owner_name, IDMAP_NAMESZ);
		if (owner_namelen < 0) {
			dprintk("nfs: couldn't resolve uid %d to string\n",
					from_kuid(&init_user_ns, iap->ia_uid));
			/* XXX */
			strcpy(owner_name, "nobody");
			owner_namelen = sizeof("nobody") - 1;
			/* goto out; */
		}
		bmval[1] |= FATTR4_WORD1_OWNER;
		len += 4 + (XDR_QUADLEN(owner_namelen) << 2);
	}
	if ((iap->ia_valid & ATTR_GID) &&
	   (attrmask[1] & FATTR4_WORD1_OWNER_GROUP)) {
		owner_grouplen = nfs_map_gid_to_group(server, iap->ia_gid, owner_group, IDMAP_NAMESZ);
		if (owner_grouplen < 0) {
			dprintk("nfs: couldn't resolve gid %d to string\n",
					from_kgid(&init_user_ns, iap->ia_gid));
			strcpy(owner_group, "nobody");
			owner_grouplen = sizeof("nobody") - 1;
			/* goto out; */
		}
		bmval[1] |= FATTR4_WORD1_OWNER_GROUP;
		len += 4 + (XDR_QUADLEN(owner_grouplen) << 2);
	}
	if (attrmask[1] & FATTR4_WORD1_TIME_ACCESS_SET) {
		if (iap->ia_valid & ATTR_ATIME_SET) {
			bmval[1] |= FATTR4_WORD1_TIME_ACCESS_SET;
			len += 4 + (nfstime4_maxsz << 2);
		} else if (iap->ia_valid & ATTR_ATIME) {
			bmval[1] |= FATTR4_WORD1_TIME_ACCESS_SET;
			len += 4;
		}
	}
	if (attrmask[1] & FATTR4_WORD1_TIME_MODIFY_SET) {
		if (iap->ia_valid & ATTR_MTIME_SET) {
			bmval[1] |= FATTR4_WORD1_TIME_MODIFY_SET;
			len += 4 + (nfstime4_maxsz << 2);
		} else if (iap->ia_valid & ATTR_MTIME) {
			bmval[1] |= FATTR4_WORD1_TIME_MODIFY_SET;
			len += 4;
		}
	}

	if (label && (attrmask[2] & FATTR4_WORD2_SECURITY_LABEL)) {
		len += 4 + 4 + 4 + (XDR_QUADLEN(label->len) << 2);
		bmval[2] |= FATTR4_WORD2_SECURITY_LABEL;
	}

	xdr_encode_bitmap4(xdr, bmval, ARRAY_SIZE(bmval));
	xdr_stream_encode_opaque_inline(xdr, (void **)&p, len);

	if (bmval[0] & FATTR4_WORD0_SIZE)
		p = xdr_encode_hyper(p, iap->ia_size);
	if (bmval[1] & FATTR4_WORD1_MODE)
		*p++ = cpu_to_be32(iap->ia_mode & S_IALLUGO);
	if (bmval[1] & FATTR4_WORD1_OWNER)
		p = xdr_encode_opaque(p, owner_name, owner_namelen);
	if (bmval[1] & FATTR4_WORD1_OWNER_GROUP)
		p = xdr_encode_opaque(p, owner_group, owner_grouplen);
	if (bmval[1] & FATTR4_WORD1_TIME_ACCESS_SET) {
		if (iap->ia_valid & ATTR_ATIME_SET) {
			*p++ = cpu_to_be32(NFS4_SET_TO_CLIENT_TIME);
			p = xdr_encode_nfstime4(p, &iap->ia_atime);
		} else
			*p++ = cpu_to_be32(NFS4_SET_TO_SERVER_TIME);
	}
	if (bmval[1] & FATTR4_WORD1_TIME_MODIFY_SET) {
		if (iap->ia_valid & ATTR_MTIME_SET) {
			*p++ = cpu_to_be32(NFS4_SET_TO_CLIENT_TIME);
			p = xdr_encode_nfstime4(p, &iap->ia_mtime);
		} else
			*p++ = cpu_to_be32(NFS4_SET_TO_SERVER_TIME);
	}
	if (label && (bmval[2] & FATTR4_WORD2_SECURITY_LABEL)) {
		*p++ = cpu_to_be32(label->lfs);
		*p++ = cpu_to_be32(label->pi);
		*p++ = cpu_to_be32(label->len);
		p = xdr_encode_opaque_fixed(p, label->label, label->len);
	}
	if (bmval[2] & FATTR4_WORD2_MODE_UMASK) {
		*p++ = cpu_to_be32(iap->ia_mode & S_IALLUGO);
		*p++ = cpu_to_be32(*umask);
	}

/* out: */
}
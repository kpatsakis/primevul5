static unsigned nfs4_exclusive_attrset(struct nfs4_opendata *opendata,
				struct iattr *sattr, struct nfs4_label **label)
{
	const __u32 *bitmask = opendata->o_arg.server->exclcreat_bitmask;
	__u32 attrset[3];
	unsigned ret;
	unsigned i;

	for (i = 0; i < ARRAY_SIZE(attrset); i++) {
		attrset[i] = opendata->o_res.attrset[i];
		if (opendata->o_arg.createmode == NFS4_CREATE_EXCLUSIVE4_1)
			attrset[i] &= ~bitmask[i];
	}

	ret = (opendata->o_arg.createmode == NFS4_CREATE_EXCLUSIVE) ?
		sattr->ia_valid : 0;

	if ((attrset[1] & (FATTR4_WORD1_TIME_ACCESS|FATTR4_WORD1_TIME_ACCESS_SET))) {
		if (sattr->ia_valid & ATTR_ATIME_SET)
			ret |= ATTR_ATIME_SET;
		else
			ret |= ATTR_ATIME;
	}

	if ((attrset[1] & (FATTR4_WORD1_TIME_MODIFY|FATTR4_WORD1_TIME_MODIFY_SET))) {
		if (sattr->ia_valid & ATTR_MTIME_SET)
			ret |= ATTR_MTIME_SET;
		else
			ret |= ATTR_MTIME;
	}

	if (!(attrset[2] & FATTR4_WORD2_SECURITY_LABEL))
		*label = NULL;
	return ret;
}
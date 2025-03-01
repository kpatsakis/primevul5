static void encode_readdir(struct xdr_stream *xdr, const struct nfs4_readdir_arg *readdir, struct rpc_rqst *req, struct compound_hdr *hdr)
{
	uint32_t attrs[3] = {
		FATTR4_WORD0_RDATTR_ERROR,
		FATTR4_WORD1_MOUNTED_ON_FILEID,
	};
	uint32_t dircount = readdir->count >> 1;
	__be32 *p, verf[2];
	uint32_t attrlen = 0;
	unsigned int i;

	if (readdir->plus) {
		attrs[0] |= FATTR4_WORD0_TYPE|FATTR4_WORD0_CHANGE|FATTR4_WORD0_SIZE|
			FATTR4_WORD0_FSID|FATTR4_WORD0_FILEHANDLE|FATTR4_WORD0_FILEID;
		attrs[1] |= FATTR4_WORD1_MODE|FATTR4_WORD1_NUMLINKS|FATTR4_WORD1_OWNER|
			FATTR4_WORD1_OWNER_GROUP|FATTR4_WORD1_RAWDEV|
			FATTR4_WORD1_SPACE_USED|FATTR4_WORD1_TIME_ACCESS|
			FATTR4_WORD1_TIME_METADATA|FATTR4_WORD1_TIME_MODIFY;
		attrs[2] |= FATTR4_WORD2_SECURITY_LABEL;
		dircount >>= 1;
	}
	/* Use mounted_on_fileid only if the server supports it */
	if (!(readdir->bitmask[1] & FATTR4_WORD1_MOUNTED_ON_FILEID))
		attrs[0] |= FATTR4_WORD0_FILEID;
	for (i = 0; i < ARRAY_SIZE(attrs); i++) {
		attrs[i] &= readdir->bitmask[i];
		if (attrs[i] != 0)
			attrlen = i+1;
	}

	encode_op_hdr(xdr, OP_READDIR, decode_readdir_maxsz, hdr);
	encode_uint64(xdr, readdir->cookie);
	encode_nfs4_verifier(xdr, &readdir->verifier);
	p = reserve_space(xdr, 12 + (attrlen << 2));
	*p++ = cpu_to_be32(dircount);
	*p++ = cpu_to_be32(readdir->count);
	*p++ = cpu_to_be32(attrlen);
	for (i = 0; i < attrlen; i++)
		*p++ = cpu_to_be32(attrs[i]);
	memcpy(verf, readdir->verifier.data, sizeof(verf));

	dprintk("%s: cookie = %llu, verifier = %08x:%08x, bitmap = %08x:%08x:%08x\n",
			__func__,
			(unsigned long long)readdir->cookie,
			verf[0], verf[1],
			attrs[0] & readdir->bitmask[0],
			attrs[1] & readdir->bitmask[1],
			attrs[2] & readdir->bitmask[2]);
}
static int decode_attr_fileid(struct xdr_stream *xdr, uint32_t *bitmap, uint64_t *fileid)
{
	__be32 *p;
	int ret = 0;

	*fileid = 0;
	if (unlikely(bitmap[0] & (FATTR4_WORD0_FILEID - 1U)))
		return -EIO;
	if (likely(bitmap[0] & FATTR4_WORD0_FILEID)) {
		p = xdr_inline_decode(xdr, 8);
		if (unlikely(!p))
			return -EIO;
		xdr_decode_hyper(p, fileid);
		bitmap[0] &= ~FATTR4_WORD0_FILEID;
		ret = NFS_ATTR_FATTR_FILEID;
	}
	dprintk("%s: fileid=%Lu\n", __func__, (unsigned long long)*fileid);
	return ret;
}
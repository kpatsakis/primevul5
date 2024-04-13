static int decode_space_limit(struct xdr_stream *xdr,
		unsigned long *pagemod_limit)
{
	__be32 *p;
	uint32_t limit_type, nblocks, blocksize;
	u64 maxsize = 0;

	p = xdr_inline_decode(xdr, 12);
	if (unlikely(!p))
		return -EIO;
	limit_type = be32_to_cpup(p++);
	switch (limit_type) {
	case NFS4_LIMIT_SIZE:
		xdr_decode_hyper(p, &maxsize);
		break;
	case NFS4_LIMIT_BLOCKS:
		nblocks = be32_to_cpup(p++);
		blocksize = be32_to_cpup(p);
		maxsize = (uint64_t)nblocks * (uint64_t)blocksize;
	}
	maxsize >>= PAGE_SHIFT;
	*pagemod_limit = min_t(u64, maxsize, ULONG_MAX);
	return 0;
}
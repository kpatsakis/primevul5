static int decode_op_map(struct xdr_stream *xdr, struct nfs4_op_map *op_map)
{
	__be32 *p;
	uint32_t bitmap_words;
	unsigned int i;

	p = xdr_inline_decode(xdr, 4);
	if (!p)
		return -EIO;
	bitmap_words = be32_to_cpup(p++);
	if (bitmap_words > NFS4_OP_MAP_NUM_WORDS)
		return -EIO;
	p = xdr_inline_decode(xdr, 4 * bitmap_words);
	for (i = 0; i < bitmap_words; i++)
		op_map->u.words[i] = be32_to_cpup(p++);

	return 0;
}
static void encode_op_map(struct xdr_stream *xdr, const struct nfs4_op_map *op_map)
{
	unsigned int i;
	encode_uint32(xdr, NFS4_OP_MAP_NUM_WORDS);
	for (i = 0; i < NFS4_OP_MAP_NUM_WORDS; i++)
		encode_uint32(xdr, op_map->u.words[i]);
}
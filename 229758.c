encode_secinfo_no_name(struct xdr_stream *xdr,
		       const struct nfs41_secinfo_no_name_args *args,
		       struct compound_hdr *hdr)
{
	encode_op_hdr(xdr, OP_SECINFO_NO_NAME, decode_secinfo_no_name_maxsz, hdr);
	encode_uint32(xdr, args->style);
	return 0;
}
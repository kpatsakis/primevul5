static void encode_setclientid_confirm(struct xdr_stream *xdr, const struct nfs4_setclientid_res *arg, struct compound_hdr *hdr)
{
	encode_op_hdr(xdr, OP_SETCLIENTID_CONFIRM,
			decode_setclientid_confirm_maxsz, hdr);
	encode_uint64(xdr, arg->clientid);
	encode_nfs4_verifier(xdr, &arg->confirm);
}
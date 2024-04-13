encode_getdeviceinfo(struct xdr_stream *xdr,
		     const struct nfs4_getdeviceinfo_args *args,
		     struct compound_hdr *hdr)
{
	__be32 *p;

	encode_op_hdr(xdr, OP_GETDEVICEINFO, decode_getdeviceinfo_maxsz, hdr);
	p = reserve_space(xdr, NFS4_DEVICEID4_SIZE + 4 + 4);
	p = xdr_encode_opaque_fixed(p, args->pdev->dev_id.data,
				    NFS4_DEVICEID4_SIZE);
	*p++ = cpu_to_be32(args->pdev->layout_type);
	*p++ = cpu_to_be32(args->pdev->maxcount);	/* gdia_maxcount */

	p = reserve_space(xdr, 4 + 4);
	*p++ = cpu_to_be32(1);			/* bitmap length */
	*p++ = cpu_to_be32(args->notify_types);
}
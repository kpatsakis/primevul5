encode_layoutcommit(struct xdr_stream *xdr,
		    struct inode *inode,
		    const struct nfs4_layoutcommit_args *args,
		    struct compound_hdr *hdr)
{
	__be32 *p;

	dprintk("%s: lbw: %llu type: %d\n", __func__, args->lastbytewritten,
		NFS_SERVER(args->inode)->pnfs_curr_ld->id);

	encode_op_hdr(xdr, OP_LAYOUTCOMMIT, decode_layoutcommit_maxsz, hdr);
	p = reserve_space(xdr, 20);
	/* Only whole file layouts */
	p = xdr_encode_hyper(p, 0); /* offset */
	p = xdr_encode_hyper(p, args->lastbytewritten + 1);	/* length */
	*p = cpu_to_be32(0); /* reclaim */
	encode_nfs4_stateid(xdr, &args->stateid);
	if (args->lastbytewritten != U64_MAX) {
		p = reserve_space(xdr, 20);
		*p++ = cpu_to_be32(1); /* newoffset = TRUE */
		p = xdr_encode_hyper(p, args->lastbytewritten);
	} else {
		p = reserve_space(xdr, 12);
		*p++ = cpu_to_be32(0); /* newoffset = FALSE */
	}
	*p++ = cpu_to_be32(0); /* Never send time_modify_changed */
	*p++ = cpu_to_be32(NFS_SERVER(args->inode)->pnfs_curr_ld->id);/* type */

	encode_uint32(xdr, args->layoutupdate_len);
	if (args->layoutupdate_pages)
		xdr_write_pages(xdr, args->layoutupdate_pages, 0,
				args->layoutupdate_len);

	return 0;
}
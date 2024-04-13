static int decode_getacl(struct xdr_stream *xdr, struct rpc_rqst *req,
			 struct nfs_getaclres *res)
{
	unsigned int savep;
	uint32_t attrlen,
		 bitmap[3] = {0};
	int status;
	unsigned int pg_offset;

	res->acl_len = 0;
	if ((status = decode_op_hdr(xdr, OP_GETATTR)) != 0)
		goto out;

	xdr_enter_page(xdr, xdr->buf->page_len);

	/* Calculate the offset of the page data */
	pg_offset = xdr->buf->head[0].iov_len;

	if ((status = decode_attr_bitmap(xdr, bitmap)) != 0)
		goto out;
	if ((status = decode_attr_length(xdr, &attrlen, &savep)) != 0)
		goto out;

	if (unlikely(bitmap[0] & (FATTR4_WORD0_ACL - 1U)))
		return -EIO;
	if (likely(bitmap[0] & FATTR4_WORD0_ACL)) {

		/* The bitmap (xdr len + bitmaps) and the attr xdr len words
		 * are stored with the acl data to handle the problem of
		 * variable length bitmaps.*/
		res->acl_data_offset = xdr_stream_pos(xdr) - pg_offset;
		res->acl_len = attrlen;

		/* Check for receive buffer overflow */
		if (res->acl_len > (xdr->nwords << 2) ||
		    res->acl_len + res->acl_data_offset > xdr->buf->page_len) {
			res->acl_flags |= NFS4_ACL_TRUNC;
			dprintk("NFS: acl reply: attrlen %u > page_len %u\n",
					attrlen, xdr->nwords << 2);
		}
	} else
		status = -EOPNOTSUPP;

out:
	return status;
}
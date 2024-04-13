static int decode_getdeviceinfo(struct xdr_stream *xdr,
				struct nfs4_getdeviceinfo_res *res)
{
	struct pnfs_device *pdev = res->pdev;
	__be32 *p;
	uint32_t len, type;
	int status;

	status = decode_op_hdr(xdr, OP_GETDEVICEINFO);
	if (status) {
		if (status == -ETOOSMALL) {
			p = xdr_inline_decode(xdr, 4);
			if (unlikely(!p))
				return -EIO;
			pdev->mincount = be32_to_cpup(p);
			dprintk("%s: Min count too small. mincnt = %u\n",
				__func__, pdev->mincount);
		}
		return status;
	}

	p = xdr_inline_decode(xdr, 8);
	if (unlikely(!p))
		return -EIO;
	type = be32_to_cpup(p++);
	if (type != pdev->layout_type) {
		dprintk("%s: layout mismatch req: %u pdev: %u\n",
			__func__, pdev->layout_type, type);
		return -EINVAL;
	}
	/*
	 * Get the length of the opaque device_addr4. xdr_read_pages places
	 * the opaque device_addr4 in the xdr_buf->pages (pnfs_device->pages)
	 * and places the remaining xdr data in xdr_buf->tail
	 */
	pdev->mincount = be32_to_cpup(p);
	if (xdr_read_pages(xdr, pdev->mincount) != pdev->mincount)
		return -EIO;

	/* Parse notification bitmap, verifying that it is zero. */
	p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p))
		return -EIO;
	len = be32_to_cpup(p);
	if (len) {
		uint32_t i;

		p = xdr_inline_decode(xdr, 4 * len);
		if (unlikely(!p))
			return -EIO;

		res->notification = be32_to_cpup(p++);
		for (i = 1; i < len; i++) {
			if (be32_to_cpup(p++)) {
				dprintk("%s: unsupported notification\n",
					__func__);
				return -EIO;
			}
		}
	}
	return 0;
}
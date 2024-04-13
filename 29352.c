static int decode_filename(struct xdr_stream *xdr, char *name, u32 *length)
{
	__be32 *p;
	u32 count;

	p = xdr_inline_decode(xdr, 4);
	if (!p)
		goto out_overflow;
	count = ntoh32(net_read_uint32(p));
	if (count > 255)
		goto out_nametoolong;
	p = xdr_inline_decode(xdr, count);
	if (!p)
		goto out_overflow;
	memcpy(name, p, count);
	name[count] = 0;
	*length = count;
	return 0;

out_nametoolong:
	pr_err("%s: returned a too long filename: %u\n", __func__, count);
	return -ENAMETOOLONG;

out_overflow:
	pr_err("%s: premature end of packet\n", __func__);
	return -EIO;
}

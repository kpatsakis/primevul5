static int decode_secinfo_gss(struct xdr_stream *xdr,
			      struct nfs4_secinfo4 *flavor)
{
	u32 oid_len;
	__be32 *p;

	p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p))
		return -EIO;
	oid_len = be32_to_cpup(p);
	if (oid_len > GSS_OID_MAX_LEN)
		return -EINVAL;

	p = xdr_inline_decode(xdr, oid_len);
	if (unlikely(!p))
		return -EIO;
	memcpy(flavor->flavor_info.oid.data, p, oid_len);
	flavor->flavor_info.oid.len = oid_len;

	p = xdr_inline_decode(xdr, 8);
	if (unlikely(!p))
		return -EIO;
	flavor->flavor_info.qop = be32_to_cpup(p++);
	flavor->flavor_info.service = be32_to_cpup(p);

	return 0;
}
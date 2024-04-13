__be32 *xdr_encode_opaque(__be32 *p, const void *ptr, unsigned int nbytes)
{
	*p++ = cpu_to_be32(nbytes);
	return xdr_encode_opaque_fixed(p, ptr, nbytes);
}
xdr_encode_nfstime4(__be32 *p, const struct timespec64 *t)
{
	p = xdr_encode_hyper(p, t->tv_sec);
	*p++ = cpu_to_be32(t->tv_nsec);
	return p;
}
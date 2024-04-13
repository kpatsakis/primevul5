xdr_decode_nfstime4(__be32 *p, struct timespec64 *t)
{
	__u64 sec;

	p = xdr_decode_hyper(p, &sec);
	t-> tv_sec = sec;
	t->tv_nsec = be32_to_cpup(p++);
	return p;
}
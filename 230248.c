static inline void encode_delegation_type(struct xdr_stream *xdr, fmode_t delegation_type)
{
	__be32 *p;

	p = reserve_space(xdr, 4);
	switch (delegation_type) {
	case 0:
		*p = cpu_to_be32(NFS4_OPEN_DELEGATE_NONE);
		break;
	case FMODE_READ:
		*p = cpu_to_be32(NFS4_OPEN_DELEGATE_READ);
		break;
	case FMODE_WRITE|FMODE_READ:
		*p = cpu_to_be32(NFS4_OPEN_DELEGATE_WRITE);
		break;
	default:
		BUG();
	}
}
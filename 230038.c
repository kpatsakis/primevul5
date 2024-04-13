static inline void encode_createmode(struct xdr_stream *xdr, const struct nfs_openargs *arg)
{
	__be32 *p;

	p = reserve_space(xdr, 4);
	switch(arg->createmode) {
	case NFS4_CREATE_UNCHECKED:
		*p = cpu_to_be32(NFS4_CREATE_UNCHECKED);
		encode_attrs(xdr, arg->u.attrs, arg->label, &arg->umask,
				arg->server, arg->server->attr_bitmask);
		break;
	case NFS4_CREATE_GUARDED:
		*p = cpu_to_be32(NFS4_CREATE_GUARDED);
		encode_attrs(xdr, arg->u.attrs, arg->label, &arg->umask,
				arg->server, arg->server->attr_bitmask);
		break;
	case NFS4_CREATE_EXCLUSIVE:
		*p = cpu_to_be32(NFS4_CREATE_EXCLUSIVE);
		encode_nfs4_verifier(xdr, &arg->u.verifier);
		break;
	case NFS4_CREATE_EXCLUSIVE4_1:
		*p = cpu_to_be32(NFS4_CREATE_EXCLUSIVE4_1);
		encode_nfs4_verifier(xdr, &arg->u.verifier);
		encode_attrs(xdr, arg->u.attrs, arg->label, &arg->umask,
				arg->server, arg->server->exclcreat_bitmask);
	}
}
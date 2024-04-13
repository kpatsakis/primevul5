static void encode_nops(struct compound_hdr *hdr)
{
	WARN_ON_ONCE(hdr->nops > NFS4_MAX_OPS);
	*hdr->nops_p = htonl(hdr->nops);
}
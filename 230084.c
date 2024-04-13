static inline u32 *nfs4_bitmask(struct nfs_server *server, struct nfs4_label *label)
{
	if (label)
		return server->attr_bitmask;

	return server->attr_bitmask_nl;
}
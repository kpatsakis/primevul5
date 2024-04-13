nfs4_map_atomic_open_claim(struct nfs_server *server,
		enum open_claim_type4 claim)
{
	if (server->caps & NFS_CAP_ATOMIC_OPEN_V1)
		return claim;
	switch (claim) {
	default:
		return claim;
	case NFS4_OPEN_CLAIM_FH:
		return NFS4_OPEN_CLAIM_NULL;
	case NFS4_OPEN_CLAIM_DELEG_CUR_FH:
		return NFS4_OPEN_CLAIM_DELEGATE_CUR;
	case NFS4_OPEN_CLAIM_DELEG_PREV_FH:
		return NFS4_OPEN_CLAIM_DELEGATE_PREV;
	}
}
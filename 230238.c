static int can_open_delegated(struct nfs_delegation *delegation, fmode_t fmode,
		enum open_claim_type4 claim)
{
	if (delegation == NULL)
		return 0;
	if ((delegation->type & fmode) != fmode)
		return 0;
	switch (claim) {
	case NFS4_OPEN_CLAIM_NULL:
	case NFS4_OPEN_CLAIM_FH:
		break;
	case NFS4_OPEN_CLAIM_PREVIOUS:
		if (!test_bit(NFS_DELEGATION_NEED_RECLAIM, &delegation->flags))
			break;
		/* Fall through */
	default:
		return 0;
	}
	nfs_mark_delegation_referenced(delegation);
	return 1;
}
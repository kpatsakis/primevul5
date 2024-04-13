nfs4_recoverable_stateid(const nfs4_stateid *stateid)
{
	if (!stateid)
		return NULL;
	switch (stateid->type) {
	case NFS4_OPEN_STATEID_TYPE:
	case NFS4_LOCK_STATEID_TYPE:
	case NFS4_DELEGATION_STATEID_TYPE:
		return stateid;
	default:
		break;
	}
	return NULL;
}
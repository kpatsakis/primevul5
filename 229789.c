static bool nfs4_error_stateid_expired(int err)
{
	switch (err) {
	case -NFS4ERR_DELEG_REVOKED:
	case -NFS4ERR_ADMIN_REVOKED:
	case -NFS4ERR_BAD_STATEID:
	case -NFS4ERR_STALE_STATEID:
	case -NFS4ERR_OLD_STATEID:
	case -NFS4ERR_OPENMODE:
	case -NFS4ERR_EXPIRED:
		return true;
	}
	return false;
}
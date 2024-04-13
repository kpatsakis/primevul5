static long nfs4_update_delay(long *timeout)
{
	long ret;
	if (!timeout)
		return NFS4_POLL_RETRY_MAX;
	if (*timeout <= 0)
		*timeout = NFS4_POLL_RETRY_MIN;
	if (*timeout > NFS4_POLL_RETRY_MAX)
		*timeout = NFS4_POLL_RETRY_MAX;
	ret = *timeout;
	*timeout <<= 1;
	return ret;
}
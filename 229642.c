static int nfs4_delay(long *timeout, bool interruptible)
{
	if (interruptible)
		return nfs4_delay_interruptible(timeout);
	return nfs4_delay_killable(timeout);
}
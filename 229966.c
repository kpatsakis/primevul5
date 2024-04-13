nfs4_stat_to_errno(int stat)
{
	int i;
	for (i = 0; nfs_errtbl[i].stat != -1; i++) {
		if (nfs_errtbl[i].stat == stat)
			return nfs_errtbl[i].errno;
	}
	if (stat <= 10000 || stat > 10100) {
		/* The server is looney tunes. */
		return -EREMOTEIO;
	}
	/* If we cannot translate the error, the recovery routines should
	 * handle it.
	 * Note: remaining NFSv4 error codes have values > 10000, so should
	 * not conflict with native Linux error codes.
	 */
	return -stat;
}
static const jpc_mstabent_t *jpc_mstab_lookup(int id)
{
	const jpc_mstabent_t *mstabent;
	for (mstabent = jpc_mstab;; ++mstabent) {
		if (mstabent->id == id || mstabent->id < 0) {
			return mstabent;
		}
	}
	assert(0);
	return 0;
}
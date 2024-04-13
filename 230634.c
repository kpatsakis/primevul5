static int sev_asid_new(void)
{
	bool retry = true;
	int pos;

	mutex_lock(&sev_bitmap_lock);

	/*
	 * SEV-enabled guest must use asid from min_sev_asid to max_sev_asid.
	 */
again:
	pos = find_next_zero_bit(sev_asid_bitmap, max_sev_asid, min_sev_asid - 1);
	if (pos >= max_sev_asid) {
		if (retry && __sev_recycle_asids()) {
			retry = false;
			goto again;
		}
		mutex_unlock(&sev_bitmap_lock);
		return -EBUSY;
	}

	__set_bit(pos, sev_asid_bitmap);

	mutex_unlock(&sev_bitmap_lock);

	return pos + 1;
}
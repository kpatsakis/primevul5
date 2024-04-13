static bool __sev_recycle_asids(void)
{
	int pos;

	/* Check if there are any ASIDs to reclaim before performing a flush */
	pos = find_next_bit(sev_reclaim_asid_bitmap,
			    max_sev_asid, min_sev_asid - 1);
	if (pos >= max_sev_asid)
		return false;

	if (sev_flush_asids())
		return false;

	bitmap_xor(sev_asid_bitmap, sev_asid_bitmap, sev_reclaim_asid_bitmap,
		   max_sev_asid);
	bitmap_zero(sev_reclaim_asid_bitmap, max_sev_asid);

	return true;
}
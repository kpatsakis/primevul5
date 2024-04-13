void sev_hardware_teardown(void)
{
	if (!svm_sev_enabled())
		return;

	bitmap_free(sev_asid_bitmap);
	bitmap_free(sev_reclaim_asid_bitmap);

	sev_flush_asids();
}
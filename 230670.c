static __exit void svm_hardware_unsetup(void)
{
	int cpu;

	if (svm_sev_enabled()) {
		bitmap_free(sev_asid_bitmap);
		bitmap_free(sev_reclaim_asid_bitmap);

		sev_flush_asids();
	}

	for_each_possible_cpu(cpu)
		svm_cpu_uninit(cpu);

	__free_pages(pfn_to_page(iopm_base >> PAGE_SHIFT), IOPM_ALLOC_ORDER);
	iopm_base = 0;
}
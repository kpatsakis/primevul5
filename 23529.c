static int __init vdso_init(void)
{
	int i;

#ifdef CONFIG_PPC64
	/*
	 * Fill up the "systemcfg" stuff for backward compatiblity
	 */
	strcpy((char *)vdso_data->eye_catcher, "SYSTEMCFG:PPC64");
	vdso_data->version.major = SYSTEMCFG_MAJOR;
	vdso_data->version.minor = SYSTEMCFG_MINOR;
	vdso_data->processor = mfspr(SPRN_PVR);
	/*
	 * Fake the old platform number for pSeries and iSeries and add
	 * in LPAR bit if necessary
	 */
	vdso_data->platform = machine_is(iseries) ? 0x200 : 0x100;
	if (firmware_has_feature(FW_FEATURE_LPAR))
		vdso_data->platform |= 1;
	vdso_data->physicalMemorySize = lmb_phys_mem_size();
	vdso_data->dcache_size = ppc64_caches.dsize;
	vdso_data->dcache_line_size = ppc64_caches.dline_size;
	vdso_data->icache_size = ppc64_caches.isize;
	vdso_data->icache_line_size = ppc64_caches.iline_size;

	/* XXXOJN: Blocks should be added to ppc64_caches and used instead */
	vdso_data->dcache_block_size = ppc64_caches.dline_size;
	vdso_data->icache_block_size = ppc64_caches.iline_size;
	vdso_data->dcache_log_block_size = ppc64_caches.log_dline_size;
	vdso_data->icache_log_block_size = ppc64_caches.log_iline_size;

	/*
	 * Calculate the size of the 64 bits vDSO
	 */
	vdso64_pages = (&vdso64_end - &vdso64_start) >> PAGE_SHIFT;
	DBG("vdso64_kbase: %p, 0x%x pages\n", vdso64_kbase, vdso64_pages);
#else
	vdso_data->dcache_block_size = L1_CACHE_BYTES;
	vdso_data->dcache_log_block_size = L1_CACHE_SHIFT;
	vdso_data->icache_block_size = L1_CACHE_BYTES;
	vdso_data->icache_log_block_size = L1_CACHE_SHIFT;
#endif /* CONFIG_PPC64 */


	/*
	 * Calculate the size of the 32 bits vDSO
	 */
	vdso32_pages = (&vdso32_end - &vdso32_start) >> PAGE_SHIFT;
	DBG("vdso32_kbase: %p, 0x%x pages\n", vdso32_kbase, vdso32_pages);


	/*
	 * Setup the syscall map in the vDOS
	 */
	vdso_setup_syscall_map();

	/*
	 * Initialize the vDSO images in memory, that is do necessary
	 * fixups of vDSO symbols, locate trampolines, etc...
	 */
	if (vdso_setup()) {
		printk(KERN_ERR "vDSO setup failure, not enabled !\n");
		vdso32_pages = 0;
#ifdef CONFIG_PPC64
		vdso64_pages = 0;
#endif
		return 0;
	}

	/* Make sure pages are in the correct state */
	vdso32_pagelist = kzalloc(sizeof(struct page *) * (vdso32_pages + 2),
				  GFP_KERNEL);
	BUG_ON(vdso32_pagelist == NULL);
	for (i = 0; i < vdso32_pages; i++) {
		struct page *pg = virt_to_page(vdso32_kbase + i*PAGE_SIZE);
		ClearPageReserved(pg);
		get_page(pg);
		vdso32_pagelist[i] = pg;
	}
	vdso32_pagelist[i++] = virt_to_page(vdso_data);
	vdso32_pagelist[i] = NULL;

#ifdef CONFIG_PPC64
	vdso64_pagelist = kzalloc(sizeof(struct page *) * (vdso64_pages + 2),
				  GFP_KERNEL);
	BUG_ON(vdso64_pagelist == NULL);
	for (i = 0; i < vdso64_pages; i++) {
		struct page *pg = virt_to_page(vdso64_kbase + i*PAGE_SIZE);
		ClearPageReserved(pg);
		get_page(pg);
		vdso64_pagelist[i] = pg;
	}
	vdso64_pagelist[i++] = virt_to_page(vdso_data);
	vdso64_pagelist[i] = NULL;
#endif /* CONFIG_PPC64 */

	get_page(virt_to_page(vdso_data));

	smp_wmb();
	vdso_ready = 1;

	return 0;
}
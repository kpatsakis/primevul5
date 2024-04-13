setup_arch (char **cmdline_p)
{
	unw_init();

	ia64_patch_vtop((u64) __start___vtop_patchlist, (u64) __end___vtop_patchlist);

	*cmdline_p = __va(ia64_boot_param->command_line);
	strlcpy(boot_command_line, *cmdline_p, COMMAND_LINE_SIZE);

	efi_init();
	io_port_init();

#ifdef CONFIG_IA64_GENERIC
	/* machvec needs to be parsed from the command line
	 * before parse_early_param() is called to ensure
	 * that ia64_mv is initialised before any command line
	 * settings may cause console setup to occur
	 */
	machvec_init_from_cmdline(*cmdline_p);
#endif

	parse_early_param();

	if (early_console_setup(*cmdline_p) == 0)
		mark_bsp_online();

#ifdef CONFIG_ACPI
	/* Initialize the ACPI boot-time table parser */
	acpi_table_init();
# ifdef CONFIG_ACPI_NUMA
	acpi_numa_init();
	per_cpu_scan_finalize((cpus_weight(early_cpu_possible_map) == 0 ?
		32 : cpus_weight(early_cpu_possible_map)), additional_cpus);
# endif
#else
# ifdef CONFIG_SMP
	smp_build_cpu_map();	/* happens, e.g., with the Ski simulator */
# endif
#endif /* CONFIG_APCI_BOOT */

	find_memory();

 	/* process SAL system table: */
 	ia64_sal_init(__va(efi.sal_systab));
 
 #ifdef CONFIG_SMP
 	cpu_physical_id(0) = hard_smp_processor_id();
 #endif

	cpu_init();	/* initialize the bootstrap CPU */
	mmu_context_init();	/* initialize context_id bitmap */

	check_sal_cache_flush();

#ifdef CONFIG_ACPI
	acpi_boot_init();
#endif

#ifdef CONFIG_VT
	if (!conswitchp) {
# if defined(CONFIG_DUMMY_CONSOLE)
		conswitchp = &dummy_con;
# endif
# if defined(CONFIG_VGA_CONSOLE)
		/*
		 * Non-legacy systems may route legacy VGA MMIO range to system
		 * memory.  vga_con probes the MMIO hole, so memory looks like
		 * a VGA device to it.  The EFI memory map can tell us if it's
		 * memory so we can avoid this problem.
		 */
		if (efi_mem_type(0xA0000) != EFI_CONVENTIONAL_MEMORY)
			conswitchp = &vga_con;
# endif
	}
#endif

	/* enable IA-64 Machine Check Abort Handling unless disabled */
	if (!nomca)
		ia64_mca_init();

	platform_setup(cmdline_p);
	paging_init();
}

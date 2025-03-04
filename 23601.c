void __cpuinit cpu_init (void)
{
	int cpu = stack_smp_processor_id();
	struct tss_struct *t = &per_cpu(init_tss, cpu);
	struct orig_ist *orig_ist = &per_cpu(orig_ist, cpu);
	unsigned long v; 
	char *estacks = NULL; 
	struct task_struct *me;
	int i;

	/* CPU 0 is initialised in head64.c */
	if (cpu != 0) {
		pda_init(cpu);
		zap_low_mappings(cpu);
	} else 
		estacks = boot_exception_stacks; 

	me = current;

	if (cpu_test_and_set(cpu, cpu_initialized))
		panic("CPU#%d already initialized!\n", cpu);

	printk("Initializing CPU#%d\n", cpu);

	clear_in_cr4(X86_CR4_VME|X86_CR4_PVI|X86_CR4_TSD|X86_CR4_DE);

	/*
	 * Initialize the per-CPU GDT with the boot GDT,
	 * and set up the GDT descriptor:
	 */
	if (cpu)
 		memcpy(cpu_gdt(cpu), cpu_gdt_table, GDT_SIZE);

	cpu_gdt_descr[cpu].size = GDT_SIZE;
	asm volatile("lgdt %0" :: "m" (cpu_gdt_descr[cpu]));
	asm volatile("lidt %0" :: "m" (idt_descr));

	memset(me->thread.tls_array, 0, GDT_ENTRY_TLS_ENTRIES * 8);
	syscall_init();

	wrmsrl(MSR_FS_BASE, 0);
	wrmsrl(MSR_KERNEL_GS_BASE, 0);
	barrier(); 

	check_efer();

	/*
	 * set up and load the per-CPU TSS
	 */
	for (v = 0; v < N_EXCEPTION_STACKS; v++) {
		static const unsigned int order[N_EXCEPTION_STACKS] = {
			[0 ... N_EXCEPTION_STACKS - 1] = EXCEPTION_STACK_ORDER,
			[DEBUG_STACK - 1] = DEBUG_STACK_ORDER
		};
		if (cpu) {
			estacks = (char *)__get_free_pages(GFP_ATOMIC, order[v]);
			if (!estacks)
				panic("Cannot allocate exception stack %ld %d\n",
				      v, cpu); 
		}
		estacks += PAGE_SIZE << order[v];
		orig_ist->ist[v] = t->ist[v] = (unsigned long)estacks;
	}

	t->io_bitmap_base = offsetof(struct tss_struct, io_bitmap);
	/*
	 * <= is required because the CPU will access up to
	 * 8 bits beyond the end of the IO permission bitmap.
	 */
	for (i = 0; i <= IO_BITMAP_LONGS; i++)
		t->io_bitmap[i] = ~0UL;

	atomic_inc(&init_mm.mm_count);
	me->active_mm = &init_mm;
	if (me->mm)
		BUG();
	enter_lazy_tlb(&init_mm, me);

	set_tss_desc(cpu, t);
	load_TR_desc();
	load_LDT(&init_mm.context);

	/*
	 * Clear all 6 debug registers:
	 */

	set_debugreg(0UL, 0);
	set_debugreg(0UL, 1);
	set_debugreg(0UL, 2);
	set_debugreg(0UL, 3);
	set_debugreg(0UL, 6);
	set_debugreg(0UL, 7);

	fpu_init(); 

	raw_local_save_flags(kernel_eflags);
}
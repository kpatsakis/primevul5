void __init udp_init(void)
{
	unsigned long limit;
	unsigned int i;

	udp_table_init(&udp_table, "UDP");
	limit = nr_free_buffer_pages() / 8;
	limit = max(limit, 128UL);
	sysctl_udp_mem[0] = limit / 4 * 3;
	sysctl_udp_mem[1] = limit;
	sysctl_udp_mem[2] = sysctl_udp_mem[0] * 2;

	sysctl_udp_rmem_min = SK_MEM_QUANTUM;
	sysctl_udp_wmem_min = SK_MEM_QUANTUM;

	/* 16 spinlocks per cpu */
	udp_busylocks_log = ilog2(nr_cpu_ids) + 4;
	udp_busylocks = kmalloc(sizeof(spinlock_t) << udp_busylocks_log,
				GFP_KERNEL);
	if (!udp_busylocks)
		panic("UDP: failed to alloc udp_busylocks\n");
	for (i = 0; i < (1U << udp_busylocks_log); i++)
		spin_lock_init(udp_busylocks + i);
}
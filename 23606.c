void __init setup_per_cpu_areas(void)
{ 
	int i;
	unsigned long size;

#ifdef CONFIG_HOTPLUG_CPU
	prefill_possible_map();
#endif

	/* Copy section for each CPU (we discard the original) */
	size = PERCPU_ENOUGH_ROOM;

	printk(KERN_INFO "PERCPU: Allocating %lu bytes of per cpu data\n", size);
	for_each_cpu_mask (i, cpu_possible_map) {
		char *ptr;

		if (!NODE_DATA(cpu_to_node(i))) {
			printk("cpu with no node %d, num_online_nodes %d\n",
			       i, num_online_nodes());
			ptr = alloc_bootmem(size);
		} else { 
			ptr = alloc_bootmem_node(NODE_DATA(cpu_to_node(i)), size);
		}
		if (!ptr)
			panic("Cannot allocate cpu data for CPU %d\n", i);
		cpu_pda(i)->data_offset = ptr - __per_cpu_start;
		memcpy(ptr, __per_cpu_start, __per_cpu_end - __per_cpu_start);
	}
} 
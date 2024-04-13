unsigned long change_prot_numa(struct vm_area_struct *vma,
			unsigned long addr, unsigned long end)
{
	int nr_updated;

	nr_updated = change_protection(vma, addr, end, PAGE_NONE, 0, 1);
	if (nr_updated)
		count_vm_numa_events(NUMA_PTE_UPDATES, nr_updated);

	return nr_updated;
}
int __pte_alloc(struct mm_struct *mm, pmd_t *pmd, unsigned long address)
{
	pgtable_t new = pte_alloc_one(mm, address);
	if (!new)
		return -ENOMEM;

	/*
	 * Ensure all pte setup (eg. pte page lock and page clearing) are
	 * visible before the pte is made visible to other CPUs by being
	 * put into page tables.
	 *
	 * The other side of the story is the pointer chasing in the page
	 * table walking code (when walking the page table without locking;
	 * ie. most of the time). Fortunately, these data accesses consist
	 * of a chain of data-dependent loads, meaning most CPUs (alpha
	 * being the notable exception) will already guarantee loads are
	 * seen in-order. See the alpha page table accessors for the
	 * smp_read_barrier_depends() barriers in page table walking code.
	 */
	smp_wmb(); /* Could be smp_wmb__xxx(before|after)_spin_lock */

	spin_lock(&mm->page_table_lock);
	if (!pmd_present(*pmd)) {	/* Has another populated it ? */
		mm->nr_ptes++;
		pmd_populate(mm, pmd, new);
		new = NULL;
	}
	spin_unlock(&mm->page_table_lock);
	if (new)
		pte_free(mm, new);
	return 0;
}
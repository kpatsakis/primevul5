static void free_pte_range(struct mmu_gather *tlb, pmd_t *pmd)
{
	pgtable_t token = pmd_pgtable(*pmd);
	pmd_clear(pmd);
	pte_free_tlb(tlb, token);
	tlb->mm->nr_ptes--;
}
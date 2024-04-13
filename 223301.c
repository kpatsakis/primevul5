int zap_huge_pud(struct mmu_gather *tlb, struct vm_area_struct *vma,
		 pud_t *pud, unsigned long addr)
{
	spinlock_t *ptl;

	ptl = __pud_trans_huge_lock(pud, vma);
	if (!ptl)
		return 0;
	/*
	 * For architectures like ppc64 we look at deposited pgtable
	 * when calling pudp_huge_get_and_clear. So do the
	 * pgtable_trans_huge_withdraw after finishing pudp related
	 * operations.
	 */
	pudp_huge_get_and_clear_full(tlb->mm, addr, pud, tlb->fullmm);
	tlb_remove_pud_tlb_entry(tlb, pud, addr);
	if (vma_is_special_huge(vma)) {
		spin_unlock(ptl);
		/* No zero page support yet */
	} else {
		/* No support for anonymous PUD pages yet */
		BUG();
	}
	return 1;
}
static void __split_huge_pud_locked(struct vm_area_struct *vma, pud_t *pud,
		unsigned long haddr)
{
	VM_BUG_ON(haddr & ~HPAGE_PUD_MASK);
	VM_BUG_ON_VMA(vma->vm_start > haddr, vma);
	VM_BUG_ON_VMA(vma->vm_end < haddr + HPAGE_PUD_SIZE, vma);
	VM_BUG_ON(!pud_trans_huge(*pud) && !pud_devmap(*pud));

	count_vm_event(THP_SPLIT_PUD);

	pudp_huge_clear_flush_notify(vma, haddr, pud);
}
void split_huge_pmd_address(struct vm_area_struct *vma, unsigned long address,
		bool freeze, struct page *page)
{
	pgd_t *pgd;
	p4d_t *p4d;
	pud_t *pud;
	pmd_t *pmd;

	pgd = pgd_offset(vma->vm_mm, address);
	if (!pgd_present(*pgd))
		return;

	p4d = p4d_offset(pgd, address);
	if (!p4d_present(*p4d))
		return;

	pud = pud_offset(p4d, address);
	if (!pud_present(*pud))
		return;

	pmd = pmd_offset(pud, address);

	__split_huge_pmd(vma, pmd, address, freeze, page);
}
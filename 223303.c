struct page *follow_devmap_pud(struct vm_area_struct *vma, unsigned long addr,
		pud_t *pud, int flags, struct dev_pagemap **pgmap)
{
	unsigned long pfn = pud_pfn(*pud);
	struct mm_struct *mm = vma->vm_mm;
	struct page *page;

	assert_spin_locked(pud_lockptr(mm, pud));

	if (flags & FOLL_WRITE && !pud_write(*pud))
		return NULL;

	/* FOLL_GET and FOLL_PIN are mutually exclusive. */
	if (WARN_ON_ONCE((flags & (FOLL_PIN | FOLL_GET)) ==
			 (FOLL_PIN | FOLL_GET)))
		return NULL;

	if (pud_present(*pud) && pud_devmap(*pud))
		/* pass */;
	else
		return NULL;

	if (flags & FOLL_TOUCH)
		touch_pud(vma, addr, pud, flags);

	/*
	 * device mapped pages can only be returned if the
	 * caller will manage the page reference count.
	 *
	 * At least one of FOLL_GET | FOLL_PIN must be set, so assert that here:
	 */
	if (!(flags & (FOLL_GET | FOLL_PIN)))
		return ERR_PTR(-EEXIST);

	pfn += (addr & ~PUD_MASK) >> PAGE_SHIFT;
	*pgmap = get_dev_pagemap(pfn, *pgmap);
	if (!*pgmap)
		return ERR_PTR(-EFAULT);
	page = pfn_to_page(pfn);
	if (!try_grab_page(page, flags))
		page = ERR_PTR(-ENOMEM);

	return page;
}
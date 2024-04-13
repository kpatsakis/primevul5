static int madvise_cold_or_pageout_pte_range(pmd_t *pmd,
				unsigned long addr, unsigned long end,
				struct mm_walk *walk)
{
	struct madvise_walk_private *private = walk->private;
	struct mmu_gather *tlb = private->tlb;
	bool pageout = private->pageout;
	struct mm_struct *mm = tlb->mm;
	struct vm_area_struct *vma = walk->vma;
	pte_t *orig_pte, *pte, ptent;
	spinlock_t *ptl;
	struct page *page = NULL;
	LIST_HEAD(page_list);

	if (fatal_signal_pending(current))
		return -EINTR;

#ifdef CONFIG_TRANSPARENT_HUGEPAGE
	if (pmd_trans_huge(*pmd)) {
		pmd_t orig_pmd;
		unsigned long next = pmd_addr_end(addr, end);

		tlb_change_page_size(tlb, HPAGE_PMD_SIZE);
		ptl = pmd_trans_huge_lock(pmd, vma);
		if (!ptl)
			return 0;

		orig_pmd = *pmd;
		if (is_huge_zero_pmd(orig_pmd))
			goto huge_unlock;

		if (unlikely(!pmd_present(orig_pmd))) {
			VM_BUG_ON(thp_migration_supported() &&
					!is_pmd_migration_entry(orig_pmd));
			goto huge_unlock;
		}

		page = pmd_page(orig_pmd);

		/* Do not interfere with other mappings of this page */
		if (page_mapcount(page) != 1)
			goto huge_unlock;

		if (next - addr != HPAGE_PMD_SIZE) {
			int err;

			get_page(page);
			spin_unlock(ptl);
			lock_page(page);
			err = split_huge_page(page);
			unlock_page(page);
			put_page(page);
			if (!err)
				goto regular_page;
			return 0;
		}

		if (pmd_young(orig_pmd)) {
			pmdp_invalidate(vma, addr, pmd);
			orig_pmd = pmd_mkold(orig_pmd);

			set_pmd_at(mm, addr, pmd, orig_pmd);
			tlb_remove_pmd_tlb_entry(tlb, pmd, addr);
		}

		ClearPageReferenced(page);
		test_and_clear_page_young(page);
		if (pageout) {
			if (!isolate_lru_page(page)) {
				if (PageUnevictable(page))
					putback_lru_page(page);
				else
					list_add(&page->lru, &page_list);
			}
		} else
			deactivate_page(page);
huge_unlock:
		spin_unlock(ptl);
		if (pageout)
			reclaim_pages(&page_list);
		return 0;
	}

	if (pmd_trans_unstable(pmd))
		return 0;
regular_page:
#endif
	tlb_change_page_size(tlb, PAGE_SIZE);
	orig_pte = pte = pte_offset_map_lock(vma->vm_mm, pmd, addr, &ptl);
	flush_tlb_batched_pending(mm);
	arch_enter_lazy_mmu_mode();
	for (; addr < end; pte++, addr += PAGE_SIZE) {
		ptent = *pte;

		if (pte_none(ptent))
			continue;

		if (!pte_present(ptent))
			continue;

		page = vm_normal_page(vma, addr, ptent);
		if (!page)
			continue;

		/*
		 * Creating a THP page is expensive so split it only if we
		 * are sure it's worth. Split it if we are only owner.
		 */
		if (PageTransCompound(page)) {
			if (page_mapcount(page) != 1)
				break;
			get_page(page);
			if (!trylock_page(page)) {
				put_page(page);
				break;
			}
			pte_unmap_unlock(orig_pte, ptl);
			if (split_huge_page(page)) {
				unlock_page(page);
				put_page(page);
				pte_offset_map_lock(mm, pmd, addr, &ptl);
				break;
			}
			unlock_page(page);
			put_page(page);
			pte = pte_offset_map_lock(mm, pmd, addr, &ptl);
			pte--;
			addr -= PAGE_SIZE;
			continue;
		}

		/* Do not interfere with other mappings of this page */
		if (page_mapcount(page) != 1)
			continue;

		VM_BUG_ON_PAGE(PageTransCompound(page), page);

		if (pte_young(ptent)) {
			ptent = ptep_get_and_clear_full(mm, addr, pte,
							tlb->fullmm);
			ptent = pte_mkold(ptent);
			set_pte_at(mm, addr, pte, ptent);
			tlb_remove_tlb_entry(tlb, pte, addr);
		}

		/*
		 * We are deactivating a page for accelerating reclaiming.
		 * VM couldn't reclaim the page unless we clear PG_young.
		 * As a side effect, it makes confuse idle-page tracking
		 * because they will miss recent referenced history.
		 */
		ClearPageReferenced(page);
		test_and_clear_page_young(page);
		if (pageout) {
			if (!isolate_lru_page(page)) {
				if (PageUnevictable(page))
					putback_lru_page(page);
				else
					list_add(&page->lru, &page_list);
			}
		} else
			deactivate_page(page);
	}

	arch_leave_lazy_mmu_mode();
	pte_unmap_unlock(orig_pte, ptl);
	if (pageout)
		reclaim_pages(&page_list);
	cond_resched();

	return 0;
}
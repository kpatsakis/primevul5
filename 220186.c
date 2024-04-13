static int madvise_free_pte_range(pmd_t *pmd, unsigned long addr,
				unsigned long end, struct mm_walk *walk)

{
	struct mmu_gather *tlb = walk->private;
	struct mm_struct *mm = tlb->mm;
	struct vm_area_struct *vma = walk->vma;
	spinlock_t *ptl;
	pte_t *orig_pte, *pte, ptent;
	struct page *page;
	int nr_swap = 0;
	unsigned long next;

	next = pmd_addr_end(addr, end);
	if (pmd_trans_huge(*pmd))
		if (madvise_free_huge_pmd(tlb, vma, pmd, addr, next))
			goto next;

	if (pmd_trans_unstable(pmd))
		return 0;

	tlb_change_page_size(tlb, PAGE_SIZE);
	orig_pte = pte = pte_offset_map_lock(mm, pmd, addr, &ptl);
	flush_tlb_batched_pending(mm);
	arch_enter_lazy_mmu_mode();
	for (; addr != end; pte++, addr += PAGE_SIZE) {
		ptent = *pte;

		if (pte_none(ptent))
			continue;
		/*
		 * If the pte has swp_entry, just clear page table to
		 * prevent swap-in which is more expensive rather than
		 * (page allocation + zeroing).
		 */
		if (!pte_present(ptent)) {
			swp_entry_t entry;

			entry = pte_to_swp_entry(ptent);
			if (non_swap_entry(entry))
				continue;
			nr_swap--;
			free_swap_and_cache(entry);
			pte_clear_not_present_full(mm, addr, pte, tlb->fullmm);
			continue;
		}

		page = vm_normal_page(vma, addr, ptent);
		if (!page)
			continue;

		/*
		 * If pmd isn't transhuge but the page is THP and
		 * is owned by only this process, split it and
		 * deactivate all pages.
		 */
		if (PageTransCompound(page)) {
			if (page_mapcount(page) != 1)
				goto out;
			get_page(page);
			if (!trylock_page(page)) {
				put_page(page);
				goto out;
			}
			pte_unmap_unlock(orig_pte, ptl);
			if (split_huge_page(page)) {
				unlock_page(page);
				put_page(page);
				pte_offset_map_lock(mm, pmd, addr, &ptl);
				goto out;
			}
			unlock_page(page);
			put_page(page);
			pte = pte_offset_map_lock(mm, pmd, addr, &ptl);
			pte--;
			addr -= PAGE_SIZE;
			continue;
		}

		VM_BUG_ON_PAGE(PageTransCompound(page), page);

		if (PageSwapCache(page) || PageDirty(page)) {
			if (!trylock_page(page))
				continue;
			/*
			 * If page is shared with others, we couldn't clear
			 * PG_dirty of the page.
			 */
			if (page_mapcount(page) != 1) {
				unlock_page(page);
				continue;
			}

			if (PageSwapCache(page) && !try_to_free_swap(page)) {
				unlock_page(page);
				continue;
			}

			ClearPageDirty(page);
			unlock_page(page);
		}

		if (pte_young(ptent) || pte_dirty(ptent)) {
			/*
			 * Some of architecture(ex, PPC) don't update TLB
			 * with set_pte_at and tlb_remove_tlb_entry so for
			 * the portability, remap the pte with old|clean
			 * after pte clearing.
			 */
			ptent = ptep_get_and_clear_full(mm, addr, pte,
							tlb->fullmm);

			ptent = pte_mkold(ptent);
			ptent = pte_mkclean(ptent);
			set_pte_at(mm, addr, pte, ptent);
			tlb_remove_tlb_entry(tlb, pte, addr);
		}
		mark_page_lazyfree(page);
	}
out:
	if (nr_swap) {
		if (current->mm == mm)
			sync_mm_rss(mm);

		add_mm_counter(mm, MM_SWAPENTS, nr_swap);
	}
	arch_leave_lazy_mmu_mode();
	pte_unmap_unlock(orig_pte, ptl);
	cond_resched();
next:
	return 0;
}
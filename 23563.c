static unsigned long zap_pte_range(struct mmu_gather *tlb,
				struct vm_area_struct *vma, pmd_t *pmd,
				unsigned long addr, unsigned long end,
				long *zap_work, struct zap_details *details)
{
	struct mm_struct *mm = tlb->mm;
	pte_t *pte;
	spinlock_t *ptl;
	int file_rss = 0;
	int anon_rss = 0;

	pte = pte_offset_map_lock(mm, pmd, addr, &ptl);
	arch_enter_lazy_mmu_mode();
	do {
		pte_t ptent = *pte;
		if (pte_none(ptent)) {
			(*zap_work)--;
			continue;
		}

		(*zap_work) -= PAGE_SIZE;

		if (pte_present(ptent)) {
			struct page *page;

			page = vm_normal_page(vma, addr, ptent);
			if (unlikely(details) && page) {
				/*
				 * unmap_shared_mapping_pages() wants to
				 * invalidate cache without truncating:
				 * unmap shared but keep private pages.
				 */
				if (details->check_mapping &&
				    details->check_mapping != page->mapping)
					continue;
				/*
				 * Each page->index must be checked when
				 * invalidating or truncating nonlinear.
				 */
				if (details->nonlinear_vma &&
				    (page->index < details->first_index ||
				     page->index > details->last_index))
					continue;
			}
			ptent = ptep_get_and_clear_full(mm, addr, pte,
							tlb->fullmm);
			tlb_remove_tlb_entry(tlb, pte, addr);
			if (unlikely(!page))
				continue;
			if (unlikely(details) && details->nonlinear_vma
			    && linear_page_index(details->nonlinear_vma,
						addr) != page->index)
				set_pte_at(mm, addr, pte,
					   pgoff_to_pte(page->index));
			if (PageAnon(page))
				anon_rss--;
			else {
				if (pte_dirty(ptent))
					set_page_dirty(page);
				if (pte_young(ptent))
					SetPageReferenced(page);
				file_rss--;
			}
			page_remove_rmap(page, vma);
			tlb_remove_page(tlb, page);
			continue;
		}
		/*
		 * If details->check_mapping, we leave swap entries;
		 * if details->nonlinear_vma, we leave file entries.
		 */
		if (unlikely(details))
			continue;
		if (!pte_file(ptent))
			free_swap_and_cache(pte_to_swp_entry(ptent));
		pte_clear_not_present_full(mm, addr, pte, tlb->fullmm);
	} while (pte++, addr += PAGE_SIZE, (addr != end && *zap_work > 0));

	add_mm_rss(mm, file_rss, anon_rss);
	arch_leave_lazy_mmu_mode();
	pte_unmap_unlock(pte - 1, ptl);

	return addr;
}
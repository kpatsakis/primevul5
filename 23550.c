static inline void cow_user_page(struct page *dst, struct page *src, unsigned long va, struct vm_area_struct *vma)
{
	/*
	 * If the source page was a PFN mapping, we don't have
	 * a "struct page" for it. We do a best-effort copy by
	 * just copying from the original user address. If that
	 * fails, we just zero-fill it. Live with it.
	 */
	if (unlikely(!src)) {
		void *kaddr = kmap_atomic(dst, KM_USER0);
		void __user *uaddr = (void __user *)(va & PAGE_MASK);

		/*
		 * This really shouldn't fail, because the page is there
		 * in the page tables. But it might just be unreadable,
		 * in which case we just give up and fill the result with
		 * zeroes.
		 */
		if (__copy_from_user_inatomic(kaddr, uaddr, PAGE_SIZE))
			memset(kaddr, 0, PAGE_SIZE);
		kunmap_atomic(kaddr, KM_USER0);
		flush_dcache_page(dst);
	} else
		copy_user_highpage(dst, src, va, vma);
}
static inline gfp_t alloc_hugepage_direct_gfpmask(struct vm_area_struct *vma)
{
	const bool vma_madvised = !!(vma->vm_flags & VM_HUGEPAGE);

	/* Always do synchronous compaction */
	if (test_bit(TRANSPARENT_HUGEPAGE_DEFRAG_DIRECT_FLAG, &transparent_hugepage_flags))
		return GFP_TRANSHUGE | (vma_madvised ? 0 : __GFP_NORETRY);

	/* Kick kcompactd and fail quickly */
	if (test_bit(TRANSPARENT_HUGEPAGE_DEFRAG_KSWAPD_FLAG, &transparent_hugepage_flags))
		return GFP_TRANSHUGE_LIGHT | __GFP_KSWAPD_RECLAIM;

	/* Synchronous compaction if madvised, otherwise kick kcompactd */
	if (test_bit(TRANSPARENT_HUGEPAGE_DEFRAG_KSWAPD_OR_MADV_FLAG, &transparent_hugepage_flags))
		return GFP_TRANSHUGE_LIGHT |
			(vma_madvised ? __GFP_DIRECT_RECLAIM :
					__GFP_KSWAPD_RECLAIM);

	/* Only do synchronous compaction if madvised */
	if (test_bit(TRANSPARENT_HUGEPAGE_DEFRAG_REQ_MADV_FLAG, &transparent_hugepage_flags))
		return GFP_TRANSHUGE_LIGHT |
		       (vma_madvised ? __GFP_DIRECT_RECLAIM : 0);

	return GFP_TRANSHUGE_LIGHT;
}
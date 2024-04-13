static long madvise_dontneed_single_vma(struct vm_area_struct *vma,
					unsigned long start, unsigned long end)
{
	zap_page_range(vma, start, end - start);
	return 0;
}
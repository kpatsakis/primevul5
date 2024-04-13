bool vma_migratable(struct vm_area_struct *vma)
{
	if (vma->vm_flags & (VM_IO | VM_PFNMAP))
		return false;

	/*
	 * DAX device mappings require predictable access latency, so avoid
	 * incurring periodic faults.
	 */
	if (vma_is_dax(vma))
		return false;

	if (is_vm_hugetlb_page(vma) &&
		!hugepage_migration_supported(hstate_vma(vma)))
		return false;

	/*
	 * Migration allocates pages in the highest zone. If we cannot
	 * do so then migration (at least from node to node) is not
	 * possible.
	 */
	if (vma->vm_file &&
		gfp_zone(mapping_gfp_mask(vma->vm_file->f_mapping))
			< policy_zone)
		return false;
	return true;
}
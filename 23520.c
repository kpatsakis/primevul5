static void remove_anon_migration_ptes(struct page *old, struct page *new)
{
	struct anon_vma *anon_vma;
	struct vm_area_struct *vma;
	unsigned long mapping;

	mapping = (unsigned long)new->mapping;

	if (!mapping || (mapping & PAGE_MAPPING_ANON) == 0)
		return;

	/*
	 * We hold the mmap_sem lock. So no need to call page_lock_anon_vma.
	 */
	anon_vma = (struct anon_vma *) (mapping - PAGE_MAPPING_ANON);
	spin_lock(&anon_vma->lock);

	list_for_each_entry(vma, &anon_vma->head, anon_vma_node)
		remove_migration_pte(vma, old, new);

	spin_unlock(&anon_vma->lock);
}
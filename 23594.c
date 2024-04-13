static void remove_file_migration_ptes(struct page *old, struct page *new)
{
	struct vm_area_struct *vma;
	struct address_space *mapping = page_mapping(new);
	struct prio_tree_iter iter;
	pgoff_t pgoff = new->index << (PAGE_CACHE_SHIFT - PAGE_SHIFT);

	if (!mapping)
		return;

	spin_lock(&mapping->i_mmap_lock);

	vma_prio_tree_foreach(vma, &iter, &mapping->i_mmap, pgoff, pgoff)
		remove_migration_pte(vma, old, new);

	spin_unlock(&mapping->i_mmap_lock);
}
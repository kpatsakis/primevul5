static void remove_migration_ptes(struct page *old, struct page *new)
{
	if (PageAnon(new))
		remove_anon_migration_ptes(old, new);
	else
		remove_file_migration_ptes(old, new);
}
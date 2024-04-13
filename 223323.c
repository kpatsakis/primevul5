static int __init split_huge_pages_debugfs(void)
{
	debugfs_create_file("split_huge_pages", 0200, NULL, NULL,
			    &split_huge_pages_fops);
	return 0;
}
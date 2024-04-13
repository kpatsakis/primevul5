void cgit_snapshot_link(const char *name, const char *title, const char *class,
			const char *head, const char *rev,
			const char *archivename)
{
	reporevlink("snapshot", name, title, class, head, rev, archivename);
}
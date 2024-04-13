static void show_snapshot_percpu_help(struct seq_file *m)
{
	seq_puts(m, "# echo 0 > snapshot : Invalid for per_cpu snapshot file.\n");
#ifdef CONFIG_RING_BUFFER_ALLOW_SWAP
	seq_puts(m, "# echo 1 > snapshot : Allocates snapshot buffer, if not already allocated.\n"
		    "#                      Takes a snapshot of the main buffer for this cpu.\n");
#else
	seq_puts(m, "# echo 1 > snapshot : Not supported with this kernel.\n"
		    "#                     Must use main snapshot file to allocate.\n");
#endif
	seq_puts(m, "# echo 2 > snapshot : Clears this cpu's snapshot buffer (but does not allocate)\n"
		    "#                      (Doesn't have to be '2' works with any number that\n"
		    "#                       is not a '0' or '1')\n");
}
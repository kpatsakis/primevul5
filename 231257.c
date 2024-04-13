static void show_snapshot_main_help(struct seq_file *m)
{
	seq_puts(m, "# echo 0 > snapshot : Clears and frees snapshot buffer\n"
		    "# echo 1 > snapshot : Allocates snapshot buffer, if not already allocated.\n"
		    "#                      Takes a snapshot of the main buffer.\n"
		    "# echo 2 > snapshot : Clears snapshot buffer (but does not allocate or free)\n"
		    "#                      (Doesn't have to be '2' works with any number that\n"
		    "#                       is not a '0' or '1')\n");
}
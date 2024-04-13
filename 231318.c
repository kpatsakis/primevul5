static int __init boot_alloc_snapshot(char *str)
{
	allocate_snapshot = true;
	/* We also need the main ring buffer expanded */
	ring_buffer_expanded = true;
	return 1;
}
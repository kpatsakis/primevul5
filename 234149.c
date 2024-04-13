void get_random_bytes(void *buf, int nbytes)
{
	extract_entropy(&nonblocking_pool, buf, nbytes, 0, 0);
}
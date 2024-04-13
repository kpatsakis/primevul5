static uint32_t *nfs_add_uint64(uint32_t *p, uint64_t val)
{
	uint64_t nval = hton64(val);

	memcpy(p, &nval, 8);
	return p + 2;
}

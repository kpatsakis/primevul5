static uint32_t *nfs_add_uint32(uint32_t *p, uint32_t val)
{
	*p++ = hton32(val);
	return p;
}

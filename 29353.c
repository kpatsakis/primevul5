static uint32_t *nfs_add_filename(uint32_t *p,
		uint32_t filename_len, const char *filename)
{
	*p++ = hton32(filename_len);

	/* zero padding */
	if (filename_len & 3)
		p[filename_len / 4] = 0;

	memcpy(p, filename, filename_len);
	p += DIV_ROUND_UP(filename_len, 4);
	return p;
}

static struct uni_screen *vc_uniscr_alloc(unsigned int cols, unsigned int rows)
{
	struct uni_screen *uniscr;
	void *p;
	unsigned int memsize, i;

	/* allocate everything in one go */
	memsize = cols * rows * sizeof(char32_t);
	memsize += rows * sizeof(char32_t *);
	p = kmalloc(memsize, GFP_KERNEL);
	if (!p)
		return NULL;

	/* initial line pointers */
	uniscr = p;
	p = uniscr->lines + rows;
	for (i = 0; i < rows; i++) {
		uniscr->lines[i] = p;
		p += cols * sizeof(char32_t);
	}
	return uniscr;
}
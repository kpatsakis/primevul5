int hfsplus_block_free(struct super_block *sb, u32 offset, u32 count)
{
	struct page *page;
	struct address_space *mapping;
	__be32 *pptr, *curr, *end;
	u32 mask, len, pnr;
	int i;

	/* is there any actual work to be done? */
	if (!count)
		return 0;

	dprint(DBG_BITMAP, "block_free: %u,%u\n", offset, count);
	/* are all of the bits in range? */
	if ((offset + count) > HFSPLUS_SB(sb).total_blocks)
		return -2;

	mutex_lock(&HFSPLUS_SB(sb).alloc_file->i_mutex);
	mapping = HFSPLUS_SB(sb).alloc_file->i_mapping;
	pnr = offset / PAGE_CACHE_BITS;
	page = read_mapping_page(mapping, pnr, NULL);
	pptr = kmap(page);
	curr = pptr + (offset & (PAGE_CACHE_BITS - 1)) / 32;
	end = pptr + PAGE_CACHE_BITS / 32;
	len = count;

	/* do any partial u32 at the start */
	i = offset % 32;
	if (i) {
		int j = 32 - i;
		mask = 0xffffffffU << j;
		if (j > count) {
			mask |= 0xffffffffU >> (i + count);
			*curr++ &= cpu_to_be32(mask);
			goto out;
		}
		*curr++ &= cpu_to_be32(mask);
		count -= j;
	}

	/* do full u32s */
	while (1) {
		while (curr < end) {
			if (count < 32)
				goto done;
			*curr++ = 0;
			count -= 32;
		}
		if (!count)
			break;
		set_page_dirty(page);
		kunmap(page);
		page = read_mapping_page(mapping, ++pnr, NULL);
		pptr = kmap(page);
		curr = pptr;
		end = pptr + PAGE_CACHE_BITS / 32;
	}
done:
	/* do any partial u32 at end */
	if (count) {
		mask = 0xffffffffU >> count;
		*curr &= cpu_to_be32(mask);
	}
out:
	set_page_dirty(page);
	kunmap(page);
	HFSPLUS_SB(sb).free_blocks += len;
	sb->s_dirt = 1;
	mutex_unlock(&HFSPLUS_SB(sb).alloc_file->i_mutex);

	return 0;
}
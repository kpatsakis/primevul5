static size_t mask_bitmap4(const __u32 *bitmap, const __u32 *mask,
		__u32 *res, size_t len)
{
	size_t i;
	__u32 tmp;

	while (len > 0 && (bitmap[len-1] == 0 || mask[len-1] == 0))
		len--;
	for (i = len; i-- > 0;) {
		tmp = bitmap[i] & mask[i];
		res[i] = tmp;
	}
	return len;
}
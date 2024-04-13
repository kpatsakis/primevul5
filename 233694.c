void apply_mask(char *perm, const char *mask)
{
	while (*perm) {
		if (*mask == '-' && *perm >= 'a' && *perm <= 'z')
			*perm = *perm - 'a' + 'A';
		perm++;
		if (*mask)
			mask++;
	}
}
utf16be_to_unicode(uint32_t *pwc, const char *s, size_t n)
{
	return (utf16_to_unicode(pwc, s, n, 1));
}
table_mailaddr_match(const char *s1, const char *s2)
{
	struct mailaddr m1;
	struct mailaddr m2;

	if (!text_to_mailaddr(&m1, s1))
		return 0;
	if (!text_to_mailaddr(&m2, s2))
		return 0;
	return mailaddr_match(&m1, &m2);
}
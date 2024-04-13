pushstr(struct evalstring ***end, struct evalstring *str)
{
	str->next = NULL;
	**end = str;
	*end = &str->next;
}
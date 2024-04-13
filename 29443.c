static int name_from_numeric(struct address buf[static 1], const char *name, int family)
{
	return __lookup_ipliteral(buf, name, family);
}

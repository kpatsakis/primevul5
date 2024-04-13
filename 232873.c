table_match(struct table *table, enum table_service kind, const char *key)
{
	return table_lookup(table, kind, key, NULL);
}
static struct string_list_item *lookup_path(struct string_list *list,
					    const char *path)
{
	struct string_list_item *item;

	while (path && path[0]) {
		if ((item = string_list_lookup(list, path)))
			return item;
		if (!(path = strchr(path, '/')))
			break;
		path++;
	}
	return NULL;
}
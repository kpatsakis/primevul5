static void string_list_free(string_list* list)
{
	/* Note: we don't free the contents of the strings array: this */
	/* is handled by the caller,  either by returning this */
	/* content,  or freeing it itself. */
	free(list->strings);
}
static void string_list_allocate(string_list* list, int allocate_count)
{
	if (!list->strings && list->allocated == 0)
	{
		list->strings = calloc((size_t)allocate_count, sizeof(char*));
		list->allocated = list->strings ? allocate_count : -1;
		list->count = 0;
	}
}
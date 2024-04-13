static void object_list_allocate(object_list* list, int allocate_count)
{
	if (!list->strings && list->allocated == 0)
	{
		list->strings = calloc(allocate_count, sizeof(list->strings[0]));
		list->allocated = list->strings ? allocate_count : -1;
		list->count = 0;
	}
}
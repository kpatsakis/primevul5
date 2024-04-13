static void string_list_initialize(string_list* list)
{
	list->strings = 0;
	list->allocated = 0;
	list->count = 0;
	list->maximum = INT_MAX;
}
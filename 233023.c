static void object_list_initialize(object_list* list)
{
	list->type_id = 0;
	list->strings = 0;
	list->allocated = 0;
	list->count = 0;
	list->maximum = INT_MAX;
}
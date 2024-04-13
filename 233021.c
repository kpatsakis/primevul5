static void object_list_free(object_list* list)
{
	free(list->strings);
}
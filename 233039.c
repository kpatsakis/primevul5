static int extract_othername_object_as_string(GENERAL_NAME* name, void* data, int index, int count)
{
	object_list* list = data;

	if (name->type != GEN_OTHERNAME)
	{
		return 1;
	}

	if (0 != OBJ_cmp(name->d.otherName->type_id, list->type_id))
	{
		return 1;
	}

	object_list_allocate(list, count);

	if (list->allocated <= 0)
	{
		return 0;
	}

	list->strings[list->count] = object_string(name->d.otherName->value);

	if (list->strings[list->count])
	{
		list->count++;
	}

	if (list->count >= list->maximum)
	{
		return 0;
	}

	return 1;
}
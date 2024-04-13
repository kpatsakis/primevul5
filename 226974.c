void license_free_scope_list(SCOPE_LIST* scopeList)
{
	UINT32 i;

	if (!scopeList)
		return;

	/*
	 * We must NOT call license_free_binary_blob() on each scopelist->array[i] element,
	 * because scopelist->array was allocated at once, by a single call to malloc. The elements
	 * it contains cannot be deallocated separately then.
	 * To make things clean, we must deallocate each scopelist->array[].data,
	 * and finish by deallocating scopelist->array with a single call to free().
	 */
	for (i = 0; i < scopeList->count; i++)
	{
		free(scopeList->array[i].data);
	}

	free(scopeList->array);
	free(scopeList);
}
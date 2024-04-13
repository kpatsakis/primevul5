static void license_print_scope_list(const SCOPE_LIST* scopeList)
{
	UINT32 index;
	const LICENSE_BLOB* scope;
	WLog_INFO(TAG, "ScopeList (%" PRIu32 "):", scopeList->count);

	for (index = 0; index < scopeList->count; index++)
	{
		scope = &scopeList->array[index];
		WLog_INFO(TAG, "\t%s", (const char*)scope->data);
	}
}
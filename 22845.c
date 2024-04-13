ObjectCapGetLoaded(
		   TPMI_DH_OBJECT   handle,        // IN: start handle
		   UINT32           count,         // IN: count of returned handles
		   TPML_HANDLE     *handleList     // OUT: list of handle
		   )
{
    TPMI_YES_NO          more = NO;
    UINT32               i;
    pAssert(HandleGetType(handle) == TPM_HT_TRANSIENT);
    // Initialize output handle list
    handleList->count = 0;
    // The maximum count of handles we may return is MAX_CAP_HANDLES
    if(count > MAX_CAP_HANDLES) count = MAX_CAP_HANDLES;
    // Iterate object slots to get loaded object handles
    for(i = handle - TRANSIENT_FIRST; i < MAX_LOADED_OBJECTS; i++)
	{
	    if(s_objects[i].attributes.occupied == TRUE)
		{
		    // A valid transient object can not be the copy of a persistent object
		    pAssert(s_objects[i].attributes.evict == CLEAR);
		    if(handleList->count < count)
			{
			    // If we have not filled up the return list, add this object
			    // handle to it
			    handleList->handle[handleList->count] = i + TRANSIENT_FIRST;
			    handleList->count++;
			}
		    else
			{
			    // If the return list is full but we still have loaded object
			    // available, report this and stop iterating
			    more = YES;
			    break;
			}
		}
	}
    return more;
}
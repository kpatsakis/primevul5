ObjectLoadEvict(
		TPM_HANDLE      *handle,        // IN:OUT: evict object handle.  If success, it
		// will be replace by the loaded object handle
		COMMAND_INDEX    commandIndex   // IN: the command being processed
		)
{
    TPM_RC          result;
    TPM_HANDLE      evictHandle = *handle;   // Save the evict handle
    OBJECT          *object;
    // If this is an index that references a persistent object created by
    // the platform, then return TPM_RH_HANDLE if the phEnable is FALSE
    if(*handle >= PLATFORM_PERSISTENT)
	{
	    // belongs to platform
	    if(g_phEnable == CLEAR)
		return TPM_RC_HANDLE;
	}
    // belongs to owner
    else if(gc.shEnable == CLEAR)
	return TPM_RC_HANDLE;
    // Try to allocate a slot for an object
    object = ObjectAllocateSlot(handle);
    if(object == NULL)
	return TPM_RC_OBJECT_MEMORY;
    // Copy persistent object to transient object slot.  A TPM_RC_HANDLE
    // may be returned at this point. This will mark the slot as containing
    // a transient object so that it will be flushed at the end of the
    // command
    result = NvGetEvictObject(evictHandle, object);
    // Bail out if this failed
    if(result != TPM_RC_SUCCESS)
	return result;
    // check the object to see if it is in the endorsement hierarchy
    // if it is and this is not a TPM2_EvictControl() command, indicate
    // that the hierarchy is disabled.
    // If the associated hierarchy is disabled, make it look like the
    // handle is not defined
    if(ObjectGetHierarchy(object) == TPM_RH_ENDORSEMENT
       && gc.ehEnable == CLEAR
       && GetCommandCode(commandIndex) != TPM_CC_EvictControl)
	return TPM_RC_HANDLE;
    return result;
}
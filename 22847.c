AllocateSequenceSlot(
		     TPM_HANDLE      *newHandle,     // OUT: receives the allocated handle
		     TPM2B_AUTH      *auth           // IN: the authValue for the slot
		     )
{
    HASH_OBJECT      *object = (HASH_OBJECT *)ObjectAllocateSlot(newHandle);
    //
    // Validate that the proper location of the hash state data relative to the
    // object state data. It would be good if this could have been done at compile
    // time but it can't so do it in something that can be removed after debug.
    cAssert(offsetof(HASH_OBJECT, auth) == offsetof(OBJECT, publicArea.authPolicy));
    if(object != NULL)
	{
	    // Set the common values that a sequence object shares with an ordinary object
	    // First, clear all attributes
	    MemorySet(&object->objectAttributes, 0, sizeof(TPMA_OBJECT));
	    // The type is TPM_ALG_NULL
	    object->type = TPM_ALG_NULL;
	    // This has no name algorithm and the name is the Empty Buffer
	    object->nameAlg = TPM_ALG_NULL;
	    // A sequence object is considered to be in the NULL hierarchy so it should
	    // be marked as temporary so that it can't be persisted
	    object->attributes.temporary = SET;
	    // A sequence object is DA exempt.
	    SET_ATTRIBUTE(object->objectAttributes, TPMA_OBJECT, noDA);
	    // Copy the authorization value
	    if(auth != NULL)
		object->auth = *auth;
	    else
		object->auth.t.size = 0;
	}
    return object;
}
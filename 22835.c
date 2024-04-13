ObjectContextLoad(
		  ANY_OBJECT_BUFFER   *object,        // IN: pointer to object structure in saved
		  //     context
		  TPMI_DH_OBJECT      *handle         // OUT: object handle
		  )
{
    OBJECT      *newObject = ObjectAllocateSlot(handle);
    // Try to allocate a slot for new object
    if(newObject != NULL)
	{
	    // Copy the first part of the object
	    MemoryCopy(newObject, object, offsetof(HASH_OBJECT, state));
	    // See if this is a sequence object
	    if(ObjectIsSequence(newObject))
		{
		    // If this is a sequence object, import the data
		    SequenceDataImport((HASH_OBJECT *)newObject,
				       (HASH_OBJECT_BUFFER *)object);
		}
	    else
		{
		    // Copy input object data to internal structure
		    MemoryCopy(newObject, object, sizeof(OBJECT));
		}
	}
    return newObject;
}
ObjectAllocateSlot(
		   TPMI_DH_OBJECT  *handle        // OUT: handle of allocated object
		   )
{
    OBJECT          *object = FindEmptyObjectSlot(handle);
    if(object != NULL)
	{
	    // if found, mark as occupied
	    ObjectSetInUse(object);
	}
    return object;
}
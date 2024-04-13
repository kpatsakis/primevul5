ObjectCleanupEvict(
		   void
		   )
{
    UINT32      i;
    // This has to be iterated because a command may have two handles
    // and they may both be persistent.
    // This could be made to be more efficient so that a search is not needed.
    for(i = 0; i < MAX_LOADED_OBJECTS; i++)
	{
	    // If an object is a temporary evict object, flush it from slot
	    OBJECT      *object = &s_objects[i];
	    if(object->attributes.evict == SET)
		ObjectFlush(object);
	}
    return;
}
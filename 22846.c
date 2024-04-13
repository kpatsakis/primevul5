ObjectStartup(
	      void
	      )
{
    UINT32      i;
    // object slots initialization
    for(i = 0; i < MAX_LOADED_OBJECTS; i++)
	{
	    //Set the slot to not occupied
	    ObjectFlush(&s_objects[i]);
	}
    return;
}
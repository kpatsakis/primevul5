ObjectFlushHierarchy(
		     TPMI_RH_HIERARCHY    hierarchy      // IN: hierarchy to be flush
		     )
{
    UINT16          i;
    // iterate object slots
    for(i = 0; i < MAX_LOADED_OBJECTS; i++)
	{
	    if(s_objects[i].attributes.occupied)          // If found an occupied slot
		{
		    switch(hierarchy)
			{
			  case TPM_RH_PLATFORM:
			    if(s_objects[i].attributes.ppsHierarchy == SET)
				s_objects[i].attributes.occupied = FALSE;
			    break;
			  case TPM_RH_OWNER:
			    if(s_objects[i].attributes.spsHierarchy == SET)
				s_objects[i].attributes.occupied = FALSE;
			    break;
			  case TPM_RH_ENDORSEMENT:
			    if(s_objects[i].attributes.epsHierarchy == SET)
				s_objects[i].attributes.occupied = FALSE;
			    break;
			  default:
			    FAIL(FATAL_ERROR_INTERNAL);
			    break;
			}
		}
	}
    return;
}
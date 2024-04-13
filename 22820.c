ObjectGetHierarchy(
		   OBJECT          *object         // IN :object
		   )
{
    if(object->attributes.spsHierarchy)
	{
	    return TPM_RH_OWNER;
	}
    else if(object->attributes.epsHierarchy)
	{
	    return TPM_RH_ENDORSEMENT;
	}
    else if(object->attributes.ppsHierarchy)
	{
	    return TPM_RH_PLATFORM;
	}
    else
	{
	    return TPM_RH_NULL;
	}
}
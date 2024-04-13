ObjectSetLoadedAttributes(
			  OBJECT          *object,        // IN: object attributes to finalize
			  TPM_HANDLE       parentHandle   // IN: the parent handle
			  )
{
    OBJECT              *parent = HandleToObject(parentHandle);
    TPMA_OBJECT          objectAttributes = object->publicArea.objectAttributes;
    //
    // Copy the stClear attribute from the public area. This could be overwritten
    // if the parent has stClear SET
    object->attributes.stClear =
	IS_ATTRIBUTE(objectAttributes, TPMA_OBJECT, stClear);
    // If parent handle is a permanent handle, it is a primary (unless it is NULL
    if(parent == NULL)
	{
	    object->attributes.primary = SET;
	    switch(parentHandle)
		{
		  case TPM_RH_ENDORSEMENT:
		    object->attributes.epsHierarchy = SET;
		    break;
		  case TPM_RH_OWNER:
		    object->attributes.spsHierarchy = SET;
		    break;
		  case TPM_RH_PLATFORM:
		    object->attributes.ppsHierarchy = SET;
		    break;
		  default:
		    // Treat the temporary attribute as a hierarchy
		    object->attributes.temporary = SET;
		    object->attributes.primary = CLEAR;
		    break;
		}
	}
    else
	{
	    // is this a stClear object
	    object->attributes.stClear =
		(IS_ATTRIBUTE(objectAttributes, TPMA_OBJECT, stClear)
		 || (parent->attributes.stClear == SET));
	    object->attributes.epsHierarchy = parent->attributes.epsHierarchy;
	    object->attributes.spsHierarchy = parent->attributes.spsHierarchy;
	    object->attributes.ppsHierarchy = parent->attributes.ppsHierarchy;
	    // An object is temporary if its parent is temporary or if the object
	    // is external
	    object->attributes.temporary = parent->attributes.temporary
					   || object->attributes.external;
	}
    // If this is an external object, set the QN == name but don't SET other
    // key properties ('parent' or 'derived')
    if(object->attributes.external)
	object->qualifiedName = object->name;
    else
	{
	    // check attributes for different types of parents
	    if(IS_ATTRIBUTE(objectAttributes, TPMA_OBJECT, restricted)
	       && !object->attributes.publicOnly
	       && IS_ATTRIBUTE(objectAttributes, TPMA_OBJECT, decrypt)
	       && object->publicArea.nameAlg != TPM_ALG_NULL)
		{
		    // This is a parent. If it is not a KEYEDHASH, it is an ordinary parent.
		    // Otherwise, it is a derivation parent.
		    if(object->publicArea.type == TPM_ALG_KEYEDHASH)
			object->attributes.derivation = SET;
		    else
			object->attributes.isParent = SET;
		}
	    ComputeQualifiedName(parentHandle, object->publicArea.nameAlg,
				 &object->name, &object->qualifiedName);
	}
    // Set slot occupied
    ObjectSetInUse(object);
    return;
}
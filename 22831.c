ComputeQualifiedName(
		     TPM_HANDLE       parentHandle,  // IN: parent's handle
		     TPM_ALG_ID       nameAlg,       // IN: name hash
		     TPM2B_NAME      *name,          // IN: name of the object
		     TPM2B_NAME      *qualifiedName  // OUT: qualified name of the object
		     )
{
    HASH_STATE      hashState;   // hash state
    TPM2B_NAME      parentName;
    if(parentHandle == TPM_RH_UNASSIGNED)
	{
	    MemoryCopy2B(&qualifiedName->b, &name->b, sizeof(qualifiedName->t.name));
	    *qualifiedName = *name;
	}
    else
	{
	    GetQualifiedName(parentHandle, &parentName);
	    //      QN_A = hash_A (QN of parent || NAME_A)
	    // Start hash
	    qualifiedName->t.size = CryptHashStart(&hashState, nameAlg);
	    // Add parent's qualified name
	    CryptDigestUpdate2B(&hashState, &parentName.b);
	    // Add self name
	    CryptDigestUpdate2B(&hashState, &name->b);
	    // Complete hash leaving room for the name algorithm
	    CryptHashEnd(&hashState, qualifiedName->t.size,
			 &qualifiedName->t.name[2]);
	    UINT16_TO_BYTE_ARRAY(nameAlg, qualifiedName->t.name);
	    qualifiedName->t.size += 2;
	}
    return;
}
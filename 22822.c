ObjectCreateEventSequence(
			  TPM2B_AUTH      *auth,          // IN: authValue
			  TPMI_DH_OBJECT  *newHandle      // OUT: sequence object handle
			  )
{
    HASH_OBJECT         *hashObject = AllocateSequenceSlot(newHandle, auth);
    UINT32               count;
    TPM_ALG_ID           hash;
    // See if slot allocated
    if(hashObject == NULL)
	return TPM_RC_OBJECT_MEMORY;
    // Set the event sequence attribute
    hashObject->attributes.eventSeq = SET;
    // Initialize hash states for each implemented PCR algorithms
    for(count = 0; (hash = CryptHashGetAlgByIndex(count)) != TPM_ALG_NULL; count++)
	CryptHashStart(&hashObject->state.hashState[count], hash);
    return TPM_RC_SUCCESS;
}
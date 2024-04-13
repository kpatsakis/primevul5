ObjectCreateHashSequence(
			 TPMI_ALG_HASH    hashAlg,       // IN: hash algorithm
			 TPM2B_AUTH      *auth,          // IN: authValue
			 TPMI_DH_OBJECT  *newHandle      // OUT: sequence object handle
			 )
{
    HASH_OBJECT         *hashObject = AllocateSequenceSlot(newHandle, auth);
    // See if slot allocated
    if(hashObject == NULL)
	return TPM_RC_OBJECT_MEMORY;
    // Set hash sequence bit
    hashObject->attributes.hashSeq = SET;
    // Start hash for hash sequence
    CryptHashStart(&hashObject->state.hashState[0], hashAlg);
    return TPM_RC_SUCCESS;
}
ObjectCreateHMACSequence(
			 TPMI_ALG_HASH    hashAlg,       // IN: hash algorithm
			 OBJECT          *keyObject,     // IN: the object containing the HMAC key
			 TPM2B_AUTH      *auth,          // IN: authValue
			 TPMI_DH_OBJECT  *newHandle      // OUT: HMAC sequence object handle
			 )
{
    HASH_OBJECT         *hmacObject;
    //
    // Try to allocate a slot for new object
    hmacObject = AllocateSequenceSlot(newHandle, auth);
    if(hmacObject == NULL)
	return TPM_RC_OBJECT_MEMORY;
    // Set HMAC sequence bit
    hmacObject->attributes.hmacSeq = SET;
#if !SMAC_IMPLEMENTED
    if(CryptHmacStart(&hmacObject->state.hmacState, hashAlg,
		      keyObject->sensitive.sensitive.bits.b.size,
		      keyObject->sensitive.sensitive.bits.b.buffer) == 0)
#else
	if(CryptMacStart(&hmacObject->state.hmacState,
			 &keyObject->publicArea.parameters,
			 hashAlg, &keyObject->sensitive.sensitive.any.b) == 0)
#endif // SMAC_IMPLEMENTED
	    return TPM_RC_FAILURE;
    return TPM_RC_SUCCESS;
}
ObjectTerminateEvent(
		     void
		     )
{
    HASH_OBJECT         *hashObject;
    int                  count;
    BYTE                 buffer[MAX_DIGEST_SIZE];
    hashObject = (HASH_OBJECT *)HandleToObject(g_DRTMHandle);
    // Don't assume that this is a proper sequence object
    if(hashObject->attributes.eventSeq)
	{
	    // If it is, close any open hash contexts. This is done in case
	    // the cryptographic implementation has some context values that need to be
	    // cleaned up (hygiene).
	    //
	    for(count = 0; CryptHashGetAlgByIndex(count) != TPM_ALG_NULL; count++)
		{
		    CryptHashEnd(&hashObject->state.hashState[count], 0, buffer);
		}
	    // Flush sequence object
	    FlushObject(g_DRTMHandle);
	}
    g_DRTMHandle = TPM_RH_UNASSIGNED;
}
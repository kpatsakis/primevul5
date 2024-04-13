OaepEncode(
	   TPM2B       *padded,        // OUT: the pad data
	   TPM_ALG_ID   hashAlg,       // IN: algorithm to use for padding
	   const TPM2B *label,         // IN: null-terminated string (may be NULL)
	   TPM2B       *message,       // IN: the message being padded
	   RAND_STATE  *rand           // IN: the random number generator to use
	   )
{
    INT32        padLen;
    INT32        dbSize;
    INT32        i;
    BYTE         mySeed[MAX_DIGEST_SIZE];
    BYTE        *seed = mySeed;
    INT32        hLen = CryptHashGetDigestSize(hashAlg);
    BYTE         mask[MAX_RSA_KEY_BYTES];
    BYTE        *pp;
    BYTE        *pm;
    TPM_RC       retVal = TPM_RC_SUCCESS;
    pAssert(padded != NULL && message != NULL);
    // A value of zero is not allowed because the KDF can't produce a result
    // if the digest size is zero.
    if(hLen <= 0)
	return TPM_RC_VALUE;
    // Basic size checks
    //  make sure digest isn't too big for key size
    if(padded->size < (2 * hLen) + 2)
	ERROR_RETURN(TPM_RC_HASH);
    // and that message will fit messageSize <= k - 2hLen - 2
    if(message->size > (padded->size - (2 * hLen) - 2))
	ERROR_RETURN(TPM_RC_VALUE);
    // Hash L even if it is null
    // Offset into padded leaving room for masked seed and byte of zero
    pp = &padded->buffer[hLen + 1];
    if(CryptHashBlock(hashAlg, label->size, (BYTE *)label->buffer,
		      hLen, pp) != hLen)
	ERROR_RETURN(TPM_RC_FAILURE);
    // concatenate PS of k  mLen  2hLen  2
    padLen = padded->size - message->size - (2 * hLen) - 2;
    MemorySet(&pp[hLen], 0, padLen);
    pp[hLen + padLen] = 0x01;
    padLen += 1;
    memcpy(&pp[hLen + padLen], message->buffer, message->size);
    // The total size of db = hLen + pad + mSize;
    dbSize = hLen + padLen + message->size;
    // If testing, then use the provided seed. Otherwise, use values
    // from the RNG
    CryptRandomGenerate(hLen, mySeed);
    DRBG_Generate(rand, mySeed, (UINT16)hLen);
    // mask = MGF1 (seed, nSize  hLen  1)
    CryptMGF1(dbSize, mask, hashAlg, hLen, seed);
    // Create the masked db
    pm = mask;
    for(i = dbSize; i > 0; i--)
	*pp++ ^= *pm++;
    pp = &padded->buffer[hLen + 1];
    // Run the masked data through MGF1
    if(CryptMGF1(hLen, &padded->buffer[1], hashAlg, dbSize, pp) != (unsigned)hLen)
	ERROR_RETURN(TPM_RC_VALUE);
    // Now XOR the seed to create masked seed
    pp = &padded->buffer[1];
    pm = seed;
    for(i = hLen; i > 0; i--)
	*pp++ ^= *pm++;
    // Set the first byte to zero
    padded->buffer[0] = 0x00;
 Exit:
    return retVal;
}
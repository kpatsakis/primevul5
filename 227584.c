PssEncode(
	  TPM2B           *out,       // OUT: the encoded buffer
	  TPM_ALG_ID       hashAlg,   // IN: hash algorithm for the encoding
	  TPM2B           *digest,    // IN: the digest
	  RAND_STATE      *rand       // IN: random number source
	  )
{
    UINT32               hLen = CryptHashGetDigestSize(hashAlg);
    BYTE                 salt[MAX_RSA_KEY_BYTES - 1];
    UINT16               saltSize;
    BYTE                *ps = salt;
    BYTE                *pOut;
    UINT16               mLen;
    HASH_STATE           hashState;
    // These are fatal errors indicating bad TPM firmware
    pAssert(out != NULL && hLen > 0 && digest != NULL);
    // Get the size of the mask
    mLen = (UINT16)(out->size - hLen - 1);
    // Maximum possible salt size is mask length - 1
    saltSize = mLen - 1;
    // Use the maximum salt size allowed by FIPS 186-4
    if(saltSize > hLen)
	saltSize = (UINT16)hLen;
    //using eOut for scratch space
    // Set the first 8 bytes to zero
    pOut = out->buffer;
    memset(pOut, 0, 8);
    // Get set the salt
    DRBG_Generate(rand, salt, saltSize);
    // Create the hash of the pad || input hash || salt
    CryptHashStart(&hashState, hashAlg);
    CryptDigestUpdate(&hashState, 8, pOut);
    CryptDigestUpdate2B(&hashState, digest);
    CryptDigestUpdate(&hashState, saltSize, salt);
    CryptHashEnd(&hashState, hLen, &pOut[out->size - hLen - 1]);
    // Create a mask
    if(CryptMGF1(mLen, pOut, hashAlg, hLen, &pOut[mLen]) != mLen)
	FAIL(FATAL_ERROR_INTERNAL);
    // Since this implementation uses key sizes that are all even multiples of
    // 8, just need to make sure that the most significant bit is CLEAR
    *pOut &= 0x7f;
    // Before we mess up the pOut value, set the last byte to 0xbc
    pOut[out->size - 1] = 0xbc;
    // XOR a byte of 0x01 at the position just before where the salt will be XOR'ed
    pOut = &pOut[mLen - saltSize - 1];
    *pOut++ ^= 0x01;
    // XOR the salt data into the buffer
    for(; saltSize > 0; saltSize--)
	*pOut++ ^= *ps++;
    // and we are done
    return TPM_RC_SUCCESS;
}
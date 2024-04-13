PssDecode(
	  TPM_ALG_ID   hashAlg,        // IN: hash algorithm to use for the encoding
	  TPM2B       *dIn,            // In: the digest to compare
	  TPM2B       *eIn             // IN: the encoded data
	  )
{
    UINT32           hLen = CryptHashGetDigestSize(hashAlg);
    BYTE             mask[MAX_RSA_KEY_BYTES];
    BYTE            *pm = mask;
    BYTE            *pe;
    BYTE             pad[8] = {0};
    UINT32           i;
    UINT32           mLen;
    BYTE             fail;
    TPM_RC           retVal = TPM_RC_SUCCESS;
    HASH_STATE       hashState;
    // These errors are indicative of failures due to programmer error
    pAssert(dIn != NULL && eIn != NULL);
    pe = eIn->buffer;
    // check the hash scheme
    if(hLen == 0)
	ERROR_RETURN(TPM_RC_SCHEME);
    // most significant bit must be zero
    fail = pe[0] & 0x80;
    // last byte must be 0xbc
    fail |= pe[eIn->size - 1] ^ 0xbc;
    // Use the hLen bytes at the end of the buffer to generate a mask
    // Doesn't start at the end which is a flag byte
    mLen = eIn->size - hLen - 1;
    CryptMGF1(mLen, mask, hashAlg, hLen, &pe[mLen]);
    // Clear the MSO of the mask to make it consistent with the encoding.
    mask[0] &= 0x7F;
    pAssert(mLen <= sizeof(mask));
    // XOR the data into the mask to recover the salt. This sequence
    // advances eIn so that it will end up pointing to the seed data
    // which is the hash of the signature data
    for(i = mLen; i > 0; i--)
	*pm++ ^= *pe++;
    // Find the first byte of 0x01 after a string of all 0x00
    for(pm = mask, i = mLen; i > 0; i--)
	{
	    if(*pm == 0x01)
		break;
	    else
		fail |= *pm++;
	}
    // i should not be zero
    fail |= (i == 0);
    // if we have failed, will continue using the entire mask as the salt value so
    // that the timing attacks will not disclose anything (I don't think that this
    // is a problem for TPM applications but, usually, we don't fail so this
    // doesn't cost anything).
    if(fail)
	{
	    i = mLen;
	    pm = mask;
	}
    else
	{
	    pm++;
	    i--;
	}
    // i contains the salt size and pm points to the salt. Going to use the input
    // hash and the seed to recreate the hash in the lower portion of eIn.
    CryptHashStart(&hashState, hashAlg);
    // add the pad of 8 zeros
    CryptDigestUpdate(&hashState, 8, pad);
    // add the provided digest value
    CryptDigestUpdate(&hashState, dIn->size, dIn->buffer);
    // and the salt
    CryptDigestUpdate(&hashState, i, pm);
    // get the result
    fail |= (CryptHashEnd(&hashState, hLen, mask) != hLen);
    // Compare all bytes
    for(pm = mask; hLen > 0; hLen--)
	// don't use fail = because that could skip the increment and compare
	// operations after the first failure and that gives away timing
	// information.
	fail |= *pm++ ^ *pe++;
    retVal = (fail != 0) ? TPM_RC_VALUE : TPM_RC_SUCCESS;
 Exit:
    return retVal;
}
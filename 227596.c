OaepDecode(
	   TPM2B           *dataOut,       // OUT: the recovered data
	   TPM_ALG_ID       hashAlg,       // IN: algorithm to use for padding
	   const TPM2B     *label,         // IN: null-terminated string (may be NULL)
	   TPM2B           *padded         // IN: the padded data
	   )
{
    UINT32       i;
    BYTE         seedMask[MAX_DIGEST_SIZE];
    UINT32       hLen = CryptHashGetDigestSize(hashAlg);
    BYTE         mask[MAX_RSA_KEY_BYTES];
    BYTE        *pp;
    BYTE        *pm;
    TPM_RC       retVal = TPM_RC_SUCCESS;
    // Strange size (anything smaller can't be an OAEP padded block)
    // Also check for no leading 0
    if((padded->size < (unsigned)((2 * hLen) + 2)) || (padded->buffer[0] != 0))
	ERROR_RETURN(TPM_RC_VALUE);
    // Use the hash size to determine what to put through MGF1 in order
    // to recover the seedMask
    CryptMGF1(hLen, seedMask, hashAlg, padded->size - hLen - 1,
	      &padded->buffer[hLen + 1]);
    // Recover the seed into seedMask
    pAssert(hLen <= sizeof(seedMask));
    pp = &padded->buffer[1];
    pm = seedMask;
    for(i = hLen; i > 0; i--)
	*pm++ ^= *pp++;
    // Use the seed to generate the data mask
    CryptMGF1(padded->size - hLen - 1, mask, hashAlg, hLen, seedMask);
    // Use the mask generated from seed to recover the padded data
    pp = &padded->buffer[hLen + 1];
    pm = mask;
    for(i = (padded->size - hLen - 1); i > 0; i--)
	*pm++ ^= *pp++;
    // Make sure that the recovered data has the hash of the label
    // Put trial value in the seed mask
    if((CryptHashBlock(hashAlg, label->size, (BYTE *)label->buffer,
		       hLen, seedMask)) != hLen)
	FAIL(FATAL_ERROR_INTERNAL);
    if(memcmp(seedMask, mask, hLen) != 0)
	ERROR_RETURN(TPM_RC_VALUE);
    // find the start of the data
    pm = &mask[hLen];
    for(i = (UINT32)padded->size - (2 * hLen) - 1; i > 0; i--)
	{
	    if(*pm++ != 0)
		break;
	}
    // If we ran out of data or didn't end with 0x01, then return an error
    if(i == 0 || pm[-1] != 0x01)
	ERROR_RETURN(TPM_RC_VALUE);
    // pm should be pointing at the first part of the data
    // and i is one greater than the number of bytes to move
    i--;
    if(i > dataOut->size)
	// Special exit to preserve the size of the output buffer
	return TPM_RC_VALUE;
    memcpy(dataOut->buffer, pm, i);
    dataOut->size = (UINT16)i;
 Exit:
    if(retVal != TPM_RC_SUCCESS)
	dataOut->size = 0;
    return retVal;
}
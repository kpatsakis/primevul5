RSASSA_Decode(
	      TPM_ALG_ID       hashAlg,        // IN: hash algorithm to use for the encoding
	      TPM2B           *hIn,            // In: the digest to compare
	      TPM2B           *eIn             // IN: the encoded data
	      )
{
    BYTE             fail;
    const BYTE      *der;
    BYTE            *pe;
    INT32            derSize = CryptHashGetDer(hashAlg, &der);
    INT32            hashSize = CryptHashGetDigestSize(hashAlg);
    INT32            fillSize;
    TPM_RC           retVal;
    BYTE            *digest;
    UINT16           digestSize;
    pAssert(hIn != NULL && eIn != NULL);
    pe = eIn->buffer;
    // Can't use this scheme if the algorithm doesn't have a DER string
    // defined or if the provided hash isn't the right size
    if(derSize == 0 || (unsigned)hashSize != hIn->size)
	ERROR_RETURN(TPM_RC_SCHEME);
    // Make sure that this combination will fit in the provided space
    // Since no data movement takes place, can just walk though this
    // and accept nearly random values. This can only be called from
    // CryptValidateSignature() so eInSize is known to be in range.
    fillSize = eIn->size - derSize - hashSize - 3;
    // Start checking (fail will become non-zero if any of the bytes do not have
    // the expected value.
    fail = *pe++;                   // initial byte of zero
    fail |= *pe++ ^ 1;              // byte of 0x01
    for(; fillSize > 0; fillSize--)
	fail |= *pe++ ^ 0xff;       // bunch of 0xff
    fail |= *pe++;                  // another 0
    for(; derSize > 0; derSize--)
	fail |= *pe++ ^ *der++;    // match the DER
    digestSize = hIn->size;
    digest = hIn->buffer;
    for(; digestSize > 0; digestSize--)
	fail |= *pe++ ^ *digest++; // match the hash
    retVal = (fail != 0) ? TPM_RC_VALUE : TPM_RC_SUCCESS;
 Exit:
    return retVal;
}
RSASSA_Encode(
	      TPM2B               *pOut,      // IN:OUT on in, the size of the public key
	      //        on out, the encoded area
	      TPM_ALG_ID           hashAlg,   // IN: hash algorithm for PKCS1v1_5
	      TPM2B               *hIn        // IN: digest value to encode
	      )
{
    const BYTE      *der;
    BYTE            *eOut;
    INT32            derSize = CryptHashGetDer(hashAlg, &der);
    INT32            fillSize;
    TPM_RC           retVal = TPM_RC_SUCCESS;
    // Can't use this scheme if the algorithm doesn't have a DER string defined.
    if(derSize == 0)
	ERROR_RETURN(TPM_RC_SCHEME);
    // If the digest size of 'hashAl' doesn't match the input digest size, then
    // the DER will misidentify the digest so return an error
    if(CryptHashGetDigestSize(hashAlg) != hIn->size)
	ERROR_RETURN(TPM_RC_VALUE);
    fillSize = pOut->size - derSize - hIn->size - 3;
    eOut = pOut->buffer;
    // Make sure that this combination will fit in the provided space
    if(fillSize < 8)
	ERROR_RETURN(TPM_RC_SIZE);
    // Start filling
    *eOut++ = 0; // initial byte of zero
    *eOut++ = 1; // byte of 0x01
    for(; fillSize > 0; fillSize--)
	*eOut++ = 0xff; // bunch of 0xff
    *eOut++ = 0; // another 0
    for(; derSize > 0; derSize--)
	*eOut++ = *der++;   // copy the DER
    der = hIn->buffer;
    for(fillSize = hIn->size; fillSize > 0; fillSize--)
	*eOut++ = *der++;   // copy the hash
 Exit:
    return retVal;
}
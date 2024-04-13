CryptRsaEncrypt(
		TPM2B_PUBLIC_KEY_RSA        *cOut,          // OUT: the encrypted data
		TPM2B                       *dIn,           // IN: the data to encrypt
		OBJECT                      *key,           // IN: the key used for encryption
		TPMT_RSA_DECRYPT            *scheme,        // IN: the type of padding and hash
		//     if needed
		const TPM2B                 *label,         // IN: in case it is needed
		RAND_STATE                  *rand           // IN: random number generator
		//     state (mostly for testing)
		)
{
    TPM_RC                       retVal = TPM_RC_SUCCESS;
    TPM2B_PUBLIC_KEY_RSA         dataIn;
    //
    // if the input and output buffers are the same, copy the input to a scratch
    // buffer so that things don't get messed up.
    if(dIn == &cOut->b)
	{
	    MemoryCopy2B(&dataIn.b, dIn, sizeof(dataIn.t.buffer));
	    dIn = &dataIn.b;
	}
    // All encryption schemes return the same size of data
    cOut->t.size = key->publicArea.unique.rsa.t.size;
    TEST(scheme->scheme);
    switch(scheme->scheme)
	{
	  case ALG_NULL_VALUE:  // 'raw' encryption
	      {
		  INT32            i;
		  INT32            dSize = dIn->size;
		  // dIn can have more bytes than cOut as long as the extra bytes
		  // are zero. Note: the more significant bytes of a number in a byte
		  // buffer are the bytes at the start of the array.
		  for(i = 0; (i < dSize) && (dIn->buffer[i] == 0); i++);
		  dSize -= i;
		  if(dSize > cOut->t.size)
		      ERROR_RETURN(TPM_RC_VALUE);
		  // Pad cOut with zeros if dIn is smaller
		  memset(cOut->t.buffer, 0, cOut->t.size - dSize);
		  // And copy the rest of the value
		  memcpy(&cOut->t.buffer[cOut->t.size - dSize], &dIn->buffer[i], dSize);
		  // If the size of dIn is the same as cOut dIn could be larger than
		  // the modulus. If it is, then RSAEP() will catch it.
	      }
	      break;
	  case ALG_RSAES_VALUE:
	    retVal = RSAES_PKCS1v1_5Encode(&cOut->b, dIn, rand);
	    break;
	  case ALG_OAEP_VALUE:
	    retVal = OaepEncode(&cOut->b, scheme->details.oaep.hashAlg, label, dIn,
				rand);
	    break;
	  default:
	    ERROR_RETURN(TPM_RC_SCHEME);
	    break;
	}
    // All the schemes that do padding will come here for the encryption step
    // Check that the Encoding worked
    if(retVal == TPM_RC_SUCCESS)
	// Padding OK so do the encryption
	retVal = RSAEP(&cOut->b, key);
 Exit:
    return retVal;
}
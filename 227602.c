CryptRsaSign(
	     TPMT_SIGNATURE      *sigOut,
	     OBJECT              *key,           // IN: key to use
	     TPM2B_DIGEST        *hIn,           // IN: the digest to sign
	     RAND_STATE          *rand           // IN: the random number generator
	     //      to use (mostly for testing)
	     )
{
    TPM_RC                retVal = TPM_RC_SUCCESS;
    UINT16                modSize;
    // parameter checks
    pAssert(sigOut != NULL && key != NULL && hIn != NULL);
    modSize = key->publicArea.unique.rsa.t.size;
    // for all non-null signatures, the size is the size of the key modulus
    sigOut->signature.rsapss.sig.t.size = modSize;
    TEST(sigOut->sigAlg);
    switch(sigOut->sigAlg)
	{
	  case ALG_NULL_VALUE:
	    sigOut->signature.rsapss.sig.t.size = 0;
	    return TPM_RC_SUCCESS;
	  case ALG_RSAPSS_VALUE:
	    retVal = PssEncode(&sigOut->signature.rsapss.sig.b,
			       sigOut->signature.rsapss.hash, &hIn->b, rand);
	    break;
	  case ALG_RSASSA_VALUE:
	    retVal = RSASSA_Encode(&sigOut->signature.rsassa.sig.b,
				   sigOut->signature.rsassa.hash, &hIn->b);
	    break;
	  default:
	    retVal = TPM_RC_SCHEME;
	}
    if(retVal == TPM_RC_SUCCESS)
	{
	    // Do the encryption using the private key
	    retVal = RSADP(&sigOut->signature.rsapss.sig.b, key);
	}
    return retVal;
}
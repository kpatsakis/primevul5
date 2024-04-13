RSADP(
      TPM2B           *inOut,        // IN/OUT: the value to encrypt
      OBJECT          *key           // IN: the key
      )
{
    BN_RSA_INITIALIZED(bnM, inOut);
    BN_RSA_INITIALIZED(bnN, &key->publicArea.unique.rsa);
    BN_RSA_INITIALIZED(bnP, &key->sensitive.sensitive.rsa);
    if(BnUnsignedCmp(bnM, bnN) >= 0)
	return TPM_RC_SIZE;
    // private key operation requires that private exponent be loaded
    // During self-test, this might not be the case so load it up if it hasn't
    // already done
    // been done
    if(!key->attributes.privateExp)
	CryptRsaLoadPrivateExponent(key);
    if(!RsaPrivateKeyOp(bnM, bnN, bnP, &key->privateExponent))
	FAIL(FATAL_ERROR_INTERNAL);
    BnTo2B(bnM, inOut, inOut->size);
    return TPM_RC_SUCCESS;
}
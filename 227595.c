CryptRsaValidateSignature(
			  TPMT_SIGNATURE  *sig,           // IN: signature
			  OBJECT          *key,           // IN: public modulus
			  TPM2B_DIGEST    *digest         // IN: The digest being validated
			  )
{
    TPM_RC          retVal;
    int             padding;
    EVP_PKEY       *pkey = NULL;
    EVP_PKEY_CTX   *ctx = NULL;
    const EVP_MD   *md;
    const char     *digestname;

    //
    // Fatal programming errors
    pAssert(key != NULL && sig != NULL && digest != NULL);
    switch(sig->sigAlg)
	{
	  case ALG_RSAPSS_VALUE:
	    padding = RSA_PKCS1_PSS_PADDING;
	    break;
	  case ALG_RSASSA_VALUE:
	    padding = RSA_PKCS1_PADDING;
	    break;
	  default:
	    return TPM_RC_SCHEME;
	}
    // Errors that might be caused by calling parameters
    if(sig->signature.rsassa.sig.t.size != key->publicArea.unique.rsa.t.size)
	ERROR_RETURN(TPM_RC_SIGNATURE);
    TEST(sig->sigAlg);

    retVal = InitOpenSSLRSAPublicKey(key, &pkey);
    if (retVal != TPM_RC_SUCCESS)
        return retVal;

    digestname = GetDigestNameByHashAlg(sig->signature.any.hashAlg);
    if (digestname == NULL)
        ERROR_RETURN(TPM_RC_VALUE);

    md = EVP_get_digestbyname(digestname);
    ctx = EVP_PKEY_CTX_new(pkey, NULL);
    if (md == NULL || ctx == NULL ||
        EVP_PKEY_verify_init(ctx) <= 0)
        ERROR_RETURN(TPM_RC_FAILURE);

    if (EVP_PKEY_CTX_set_rsa_padding(ctx, padding) <= 0 ||
        EVP_PKEY_CTX_set_signature_md(ctx, md) <= 0)
        ERROR_RETURN(TPM_RC_FAILURE);

    if (EVP_PKEY_verify(ctx,
                        sig->signature.rsassa.sig.t.buffer, sig->signature.rsassa.sig.t.size,
                        digest->t.buffer, digest->t.size) <= 0)
        ERROR_RETURN(TPM_RC_SIGNATURE);

    retVal = TPM_RC_SUCCESS;

 Exit:
    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);

    return (retVal != TPM_RC_SUCCESS) ? TPM_RC_SIGNATURE : TPM_RC_SUCCESS;
}
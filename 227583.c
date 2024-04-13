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
    size_t                outlen;
    int                   padding;
    EVP_PKEY             *pkey = NULL;
    EVP_PKEY_CTX         *ctx = NULL;
    const EVP_MD         *md;
    const char           *digestname;
    TPMI_ALG_HASH         hashAlg;

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
            padding = RSA_PKCS1_PSS_PADDING;
            hashAlg = sigOut->signature.rsapss.hash;
            break;
          case ALG_RSASSA_VALUE:
            padding = RSA_PKCS1_PADDING;
            hashAlg = sigOut->signature.rsassa.hash;
            break;
          default:
            ERROR_RETURN(TPM_RC_SCHEME);
         }

    digestname = GetDigestNameByHashAlg(hashAlg);
    if (digestname == NULL)
        ERROR_RETURN(TPM_RC_VALUE);

    md = EVP_get_digestbyname(digestname);
    if (md == NULL)
        ERROR_RETURN(TPM_RC_FAILURE);

    retVal = InitOpenSSLRSAPrivateKey(key, &pkey);
    if (retVal != TPM_RC_SUCCESS)
        return retVal;

    ctx = EVP_PKEY_CTX_new(pkey, NULL);
    if (ctx == NULL ||
        EVP_PKEY_sign_init(ctx) <= 0)
        ERROR_RETURN(TPM_RC_FAILURE);

    if (EVP_PKEY_CTX_set_rsa_padding(ctx, padding) <= 0 ||
        EVP_PKEY_CTX_set_signature_md(ctx, md) <= 0)
        ERROR_RETURN(TPM_RC_FAILURE);

    outlen = sigOut->signature.rsapss.sig.t.size;
    if (EVP_PKEY_sign(ctx,
                      sigOut->signature.rsapss.sig.t.buffer, &outlen,
                      hIn->b.buffer, hIn->b.size) <= 0)
        ERROR_RETURN(TPM_RC_FAILURE);

    sigOut->signature.rsapss.sig.t.size = outlen;

 Exit:
    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);

    return retVal;
}
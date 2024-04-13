CryptRsaLoadPrivateExponent(
			    OBJECT          *rsaKey        // IN: the RSA key object
			    )
{
    BN_RSA_INITIALIZED(bnN, &rsaKey->publicArea.unique.rsa);
    BN_PRIME_INITIALIZED(bnP, &rsaKey->sensitive.sensitive.rsa);
    BN_RSA(bnQ);
    BN_PRIME(bnQr);
    BN_WORD_INITIALIZED(bnE, (rsaKey->publicArea.parameters.rsaDetail.exponent == 0)
			? RSA_DEFAULT_PUBLIC_EXPONENT
			: rsaKey->publicArea.parameters.rsaDetail.exponent);
    TPM_RC          retVal = TPM_RC_SUCCESS;
    if(!rsaKey->attributes.privateExp)
	{
	    TEST(ALG_NULL_VALUE);
	    // Make sure that the bigNum used for the exponent is properly initialized
	    RsaInitializeExponent(&rsaKey->privateExponent);
	    // Find the second prime by division
	    BnDiv(bnQ, bnQr, bnN, bnP);
	    if(!BnEqualZero(bnQr))
		ERROR_RETURN(TPM_RC_BINDING);
	    // Compute the private exponent and return it if found
	    if(!ComputePrivateExponent(bnP, bnQ, bnE, bnN,
				       &rsaKey->privateExponent))
		ERROR_RETURN(TPM_RC_BINDING);
	}
 Exit:
    rsaKey->attributes.privateExp = (retVal == TPM_RC_SUCCESS);
    return retVal;
}
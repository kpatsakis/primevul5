CryptRsaSelectScheme(
		     TPMI_DH_OBJECT       rsaHandle,     // IN: handle of an RSA key
		     TPMT_RSA_DECRYPT    *scheme         // IN: a sign or decrypt scheme
		     )
{
    OBJECT              *rsaObject;
    TPMT_ASYM_SCHEME    *keyScheme;
    TPMT_RSA_DECRYPT    *retVal = NULL;
    // Get sign object pointer
    rsaObject = HandleToObject(rsaHandle);
    keyScheme = &rsaObject->publicArea.parameters.asymDetail.scheme;
    // if the default scheme of the object is TPM_ALG_NULL, then select the
    // input scheme
    if(keyScheme->scheme == TPM_ALG_NULL)
	{
	    retVal = scheme;
	}
    // if the object scheme is not TPM_ALG_NULL and the input scheme is
    // TPM_ALG_NULL, then select the default scheme of the object.
    else if(scheme->scheme == TPM_ALG_NULL)
	{
	    // if input scheme is NULL
	    retVal = (TPMT_RSA_DECRYPT *)keyScheme;
	}
    // get here if both the object scheme and the input scheme are
    // not TPM_ALG_NULL. Need to insure that they are the same.
    // IMPLEMENTATION NOTE: This could cause problems if future versions have
    // schemes that have more values than just a hash algorithm. A new function
    // (IsSchemeSame()) might be needed then.
    else if(keyScheme->scheme == scheme->scheme
	    && keyScheme->details.anySig.hashAlg == scheme->details.anySig.hashAlg)
	{
	    retVal = scheme;
	}
    // two different, incompatible schemes specified will return NULL
    return retVal;
}
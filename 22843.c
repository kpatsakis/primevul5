ObjectLoad(
	   OBJECT          *object,        // IN: pointer to object slot
	   //     object
	   OBJECT          *parent,        // IN: (optional) the parent object
	   TPMT_PUBLIC     *publicArea,    // IN: public area to be installed in the object
	   TPMT_SENSITIVE  *sensitive,     // IN: (optional) sensitive area to be
	   //      installed in the object
	   TPM_RC           blamePublic,   // IN: parameter number to associate with the
	   //     publicArea errors
	   TPM_RC           blameSensitive,// IN: parameter number to associate with the
	   //     sensitive area errors
	   TPM2B_NAME      *name           // IN: (optional)
	   )
{
    TPM_RC           result = TPM_RC_SUCCESS;
    BOOL             doCheck;
    //
    // Do validations of public area object descriptions
    // Is this public only or a no-name object?
    if(sensitive == NULL || publicArea->nameAlg == TPM_ALG_NULL)
	{
	    // Need to have schemes checked so that we do the right thing with the
	    // public key.
	    result = SchemeChecks(NULL, publicArea);
	}
    else
	{
	    // For any sensitive area, make sure that the seedSize is no larger than the
	    // digest size of nameAlg
	    if(sensitive->seedValue.t.size
	       > CryptHashGetDigestSize(publicArea->nameAlg))
		return TPM_RCS_KEY_SIZE + blameSensitive;
	    // Check attributes and schemes for consistency
	    result = PublicAttributesValidation(parent, publicArea);
	}
    if(result != TPM_RC_SUCCESS)
	return RcSafeAddToResult(result, blamePublic);
    // If object == NULL, then this is am import. For import, load is not called
    // unless the parent is fixedTPM.
    if(object == NULL)
	doCheck = TRUE;// //
    // If the parent is not NULL, then this is an ordinary load and we only check
    // if the parent is not fixedTPM
    else if(parent != NULL)
	doCheck = !IS_ATTRIBUTE(parent->publicArea.objectAttributes,
				TPMA_OBJECT, fixedTPM);
    else
	// This is a loadExternal. Check everything.
	// Note: the check functions will filter things based on the name algorithm
	// and whether or not both parts are loaded.
	doCheck = TRUE;
    // Note: the parent will be NULL if this is a load external. CryptValidateKeys()
    // will only check the parts that need to be checked based on the settings
    // of publicOnly and nameAlg.
    // Note: For an RSA key, the keys sizes are checked but the binding is not
    // checked.
    if(doCheck)
	{
	    // Do the cryptographic key validation
	    result = CryptValidateKeys(publicArea, sensitive, blamePublic,
				       blameSensitive);
	}
    // If this is an import, we are done
    if(object == NULL || result != TPM_RC_SUCCESS)
	return result;
    // Set the name, if one was provided
    if(name != NULL)
	object->name = *name;
    else
	object->name.t.size = 0;
    // Initialize public
    object->publicArea = *publicArea;
    // If there is a sensitive area, load it
    if(sensitive == NULL)
	object->attributes.publicOnly = SET;
    else
	{
	    object->sensitive = *sensitive;
#if ALG_RSA
	    // If this is an RSA key that is not a parent, complete the load by
	    // computing the private exponent.
	    if(publicArea->type == ALG_RSA_VALUE)
		result = CryptRsaLoadPrivateExponent(object);
#endif
	}
    return result;
}
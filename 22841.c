ObjectIsStorage(
		TPMI_DH_OBJECT   handle         // IN: object handle
		)
{
    OBJECT           *object = HandleToObject(handle);
    TPMT_PUBLIC      *publicArea = ((object != NULL) ? &object->publicArea : NULL);
    //
    return (publicArea != NULL
	    && IS_ATTRIBUTE(publicArea->objectAttributes, TPMA_OBJECT, restricted)
	    && IS_ATTRIBUTE(publicArea->objectAttributes, TPMA_OBJECT, decrypt)
	    && !IS_ATTRIBUTE(publicArea->objectAttributes, TPMA_OBJECT, sign)
	    && (object->publicArea.type == ALG_RSA_VALUE
		|| object->publicArea.type == ALG_ECC_VALUE));
}
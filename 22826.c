GetQualifiedName(
		 TPMI_DH_OBJECT   handle,        // IN: handle of the object
		 TPM2B_NAME      *qualifiedName  // OUT: qualified name of the object
		 )
{
    OBJECT      *object;
    switch(HandleGetType(handle))
	{
	  case TPM_HT_PERMANENT:
	    qualifiedName->t.size = sizeof(TPM_HANDLE);
	    UINT32_TO_BYTE_ARRAY(handle, qualifiedName->t.name);
	    break;
	  case TPM_HT_TRANSIENT:
	    object = HandleToObject(handle);
	    if(object == NULL || object->publicArea.nameAlg == TPM_ALG_NULL)
		qualifiedName->t.size = 0;
	    else
		// Copy the name
		*qualifiedName = object->qualifiedName;
	    break;
	  default:
	    FAIL(FATAL_ERROR_INTERNAL);
	}
    return;
}
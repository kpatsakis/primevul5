ObjectGetPublicAttributes(
			  TPM_HANDLE       handle
			  )
{
    return HandleToObject(handle)->publicArea.objectAttributes;
}
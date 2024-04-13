ObjectGetProperties(
		    TPM_HANDLE       handle
		    )
{
    return HandleToObject(handle)->attributes;
}
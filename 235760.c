static EFI_SIGNATURE_LIST* get_esl_signature_list(const char *buf, size_t buflen)
{
	EFI_SIGNATURE_LIST *list = NULL;

	if (buflen < sizeof(EFI_SIGNATURE_LIST) || !buf)
		return NULL;

	list = (EFI_SIGNATURE_LIST *)buf;

	return list;
}
static int32_t get_esl_signature_list_size(const char *buf, const size_t buflen)
{
	EFI_SIGNATURE_LIST *list = get_esl_signature_list(buf, buflen);

	if (!list)
		return OPAL_PARAMETER;

	return le32_to_cpu(list->SignatureListSize);
}
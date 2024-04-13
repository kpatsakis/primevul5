static int get_esl_cert(const char *buf, const size_t buflen, char **cert)
{
	size_t sig_data_offset;
	size_t size;
	EFI_SIGNATURE_LIST *list = get_esl_signature_list(buf, buflen);

	if (!list)
		return OPAL_PARAMETER;

	assert(cert != NULL);

	size = le32_to_cpu(list->SignatureSize) - sizeof(uuid_t);

	prlog(PR_DEBUG,"size of signature list size is %u\n",
			le32_to_cpu(list->SignatureListSize));
	prlog(PR_DEBUG, "size of signature header size is %u\n",
			le32_to_cpu(list->SignatureHeaderSize));
	prlog(PR_DEBUG, "size of signature size is %u\n",
			le32_to_cpu(list->SignatureSize));

	sig_data_offset = sizeof(EFI_SIGNATURE_LIST)
			  + le32_to_cpu(list->SignatureHeaderSize)
			  + 16 * sizeof(uint8_t);
	if (sig_data_offset > buflen)
		return OPAL_PARAMETER;

	*cert = zalloc(size);
	if (!(*cert))
		return OPAL_NO_MEM;

	/* Since buf can have more than one ESL, copy only the size calculated
	 * to return single ESL */
	memcpy(*cert, buf + sig_data_offset, size);

	return size;
}
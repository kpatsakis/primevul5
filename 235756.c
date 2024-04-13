int validate_esl_list(const char *key, const char *esl, const size_t size)
{
	int count = 0;
	int dsize;
	char *data = NULL;
	int eslvarsize = size;
	int eslsize;
	int rc = OPAL_SUCCESS;
	int offset = 0;
	EFI_SIGNATURE_LIST *list = NULL;

	while (eslvarsize > 0) {
		prlog(PR_DEBUG, "esl var size size is %d offset is %d\n", eslvarsize, offset);
		if (eslvarsize < sizeof(EFI_SIGNATURE_LIST))
			break;

		/* Check Supported ESL Type */
		list = get_esl_signature_list(esl, eslvarsize);

		if (!list)
			return OPAL_PARAMETER;

		/* Calculate the size of the ESL */
		eslsize = le32_to_cpu(list->SignatureListSize);

		/* If could not extract the size */
		if (eslsize <= 0) {
			prlog(PR_ERR, "Invalid size of the ESL: %u\n",
					le32_to_cpu(list->SignatureListSize));
			rc = OPAL_PARAMETER;
			break;
		}

		/* Extract the certificate from the ESL */
		dsize = get_esl_cert(esl, eslvarsize, &data);
		if (dsize < 0) {
			rc = dsize;
			break;
		}

		if (key_equals(key, "dbx")) {
			if (!validate_hash(list->SignatureType, dsize)) {
				prlog(PR_ERR, "No valid hash is found\n");
				rc = OPAL_PARAMETER;
				break;
			}
		} else {
		       if (!uuid_equals(&list->SignatureType, &EFI_CERT_X509_GUID)
			   || !validate_cert(data, dsize)) {
				prlog(PR_ERR, "No valid cert is found\n");
				rc = OPAL_PARAMETER;
				break;
		       }
		}

		count++;

		/* Look for the next ESL */
		offset = offset + eslsize;
		eslvarsize = eslvarsize - eslsize;
		free(data);
		/* Since we are going to allocate again in the next iteration */
		data = NULL;
	}

	if (rc == OPAL_SUCCESS) {
		if (key_equals(key, "PK") && (count > 1)) {
			prlog(PR_ERR, "PK can only be one\n");
			rc = OPAL_PARAMETER;
		} else {
			rc = count;
		}
	}

	free(data);

	prlog(PR_INFO, "Total ESLs are %d\n", rc);
	return rc;
}
int process_update(const struct secvar *update, char **newesl,
		   int *new_data_size, struct efi_time *timestamp,
		   struct list_head *bank, char *last_timestamp)
{
	struct efi_variable_authentication_2 *auth = NULL;
	void *auth_buffer = NULL;
	int auth_buffer_size = 0;
	const char *key_authority[3];
	char *tbhbuffer = NULL;
	size_t tbhbuffersize = 0;
	struct secvar *avar = NULL;
	int rc = 0;
	int i;

	/* We need to split data into authentication descriptor and new ESL */
	auth_buffer_size = get_auth_descriptor2(update->data,
						update->data_size,
						&auth_buffer);
	if ((auth_buffer_size < 0)
	     || (update->data_size < auth_buffer_size)) {
		prlog(PR_ERR, "Invalid auth buffer size\n");
		rc = auth_buffer_size;
		goto out;
	}

	auth = auth_buffer;

	if (!timestamp) {
		rc = OPAL_INTERNAL_ERROR;
		goto out;
	}

	memcpy(timestamp, auth_buffer, sizeof(struct efi_time));

	rc = check_timestamp(update->key, timestamp, last_timestamp);
	/* Failure implies probably an older command being resubmitted */
	if (rc != OPAL_SUCCESS) {
		prlog(PR_ERR, "Timestamp verification failed for key %s\n", update->key);
		goto out;
	}

	/* Calculate the size of new ESL data */
	*new_data_size = update->data_size - auth_buffer_size;
	if (*new_data_size < 0) {
		prlog(PR_ERR, "Invalid new ESL (new data content) size\n");
		rc = OPAL_PARAMETER;
		goto out;
	}
	*newesl = zalloc(*new_data_size);
	if (!(*newesl)) {
		rc = OPAL_NO_MEM;
		goto out;
	}
	memcpy(*newesl, update->data + auth_buffer_size, *new_data_size);

	/* Validate the new ESL is in right format */
	rc = validate_esl_list(update->key, *newesl, *new_data_size);
	if (rc < 0) {
		prlog(PR_ERR, "ESL validation failed for key %s with error %04x\n",
		      update->key, rc);
		goto out;
	}

	if (setup_mode) {
		rc = OPAL_SUCCESS;
		goto out;
	}

	/* Prepare the data to be verified */
	tbhbuffer = get_hash_to_verify(update->key, *newesl, *new_data_size,
				timestamp);
	if (!tbhbuffer) {
		rc = OPAL_INTERNAL_ERROR;
		goto out;
	}

	/* Get the authority to verify the signature */
	get_key_authority(key_authority, update->key);

	/*
	 * Try for all the authorities that are allowed to sign.
	 * For eg. db/dbx can be signed by both PK or KEK
	 */
	for (i = 0; key_authority[i] != NULL; i++) {
		prlog(PR_DEBUG, "key is %s\n", update->key);
		prlog(PR_DEBUG, "key authority is %s\n", key_authority[i]);
		avar = find_secvar(key_authority[i],
				    strlen(key_authority[i]) + 1,
				    bank);
		if (!avar || !avar->data_size)
			continue;

		/* Verify the signature */
		rc = verify_signature(auth, tbhbuffer, tbhbuffersize,
				      avar);

		/* Break if signature verification is successful */
		if (rc == OPAL_SUCCESS) {
			prlog(PR_INFO, "Key %s successfully verified by authority %s\n", update->key, key_authority[i]);
			break;
		}
	}

out:
	free(auth_buffer);
	free(tbhbuffer);

	return rc;
}
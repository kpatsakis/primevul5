static char *get_hash_to_verify(const char *key, const char *new_data,
				const size_t new_data_size,
				const struct efi_time *timestamp)
{
	le32 attr = cpu_to_le32(SECVAR_ATTRIBUTES);
	size_t varlen;
	char *wkey;
	uuid_t guid;
	unsigned char *hash = NULL;
	const mbedtls_md_info_t *md_info;
	mbedtls_md_context_t ctx;
	int rc;

	md_info = mbedtls_md_info_from_type( MBEDTLS_MD_SHA256 );
	mbedtls_md_init(&ctx);

	rc = mbedtls_md_setup(&ctx, md_info, 0);
	if (rc)
		goto out;

	rc = mbedtls_md_starts(&ctx);
	if (rc)
		goto out;

	if (key_equals(key, "PK")
	    || key_equals(key, "KEK"))
		guid = EFI_GLOBAL_VARIABLE_GUID;
	else if (key_equals(key, "db")
	    || key_equals(key, "dbx"))
		guid = EFI_IMAGE_SECURITY_DATABASE_GUID;
	else
		return NULL;

	/* Expand char name to wide character width */
	varlen = strlen(key) * 2;
	wkey = char_to_wchar(key, strlen(key));
	rc = mbedtls_md_update(&ctx, wkey, varlen);
	free(wkey);
	if (rc) 
		goto out;

	rc = mbedtls_md_update(&ctx, (const unsigned char *)&guid, sizeof(guid));
	if (rc)
		goto out;

	rc = mbedtls_md_update(&ctx, (const unsigned char *)&attr, sizeof(attr));
	if (rc)
		goto out;

	rc = mbedtls_md_update(&ctx, (const unsigned char *)timestamp,
			       sizeof(struct efi_time));
	if (rc)
		goto out;

	rc = mbedtls_md_update(&ctx, new_data, new_data_size);
	if (rc)
		goto out;

	hash = zalloc(32);
	if (!hash)
		return NULL;
	rc = mbedtls_md_finish(&ctx, hash);
	if (rc) {
		free(hash);
		hash = NULL;
	}

out:
	mbedtls_md_free(&ctx);
	return hash;
}
iasecc_delete_file(struct sc_card *card, const struct sc_path *path)
{
	struct sc_context *ctx = card->ctx;
	const struct sc_acl_entry *entry = NULL;
	struct sc_apdu apdu;
	struct sc_file *file = NULL;
	int rv;

	LOG_FUNC_CALLED(ctx);
	sc_print_cache(card);

	rv = iasecc_select_file(card, path, &file);
	if (rv == SC_ERROR_FILE_NOT_FOUND)
		LOG_FUNC_RETURN(ctx, SC_SUCCESS);
	LOG_TEST_RET(ctx, rv, "Cannot select file to delete");

	entry = sc_file_get_acl_entry(file, SC_AC_OP_DELETE);
	if (!entry)
		LOG_TEST_RET(ctx, SC_ERROR_OBJECT_NOT_FOUND, "Cannot delete file: no 'DELETE' acl");

	sc_log(ctx, "DELETE method/reference %X/%X", entry->method, entry->key_ref);
	if (entry->method == SC_AC_SCB && (entry->key_ref & IASECC_SCB_METHOD_SM))   {
		unsigned char se_num = (entry->method == SC_AC_SCB) ? (entry->key_ref & IASECC_SCB_METHOD_MASK_REF) : 0;
		rv = iasecc_sm_delete_file(card, se_num, file->id);
	}
	else   {
		sc_format_apdu(card, &apdu, SC_APDU_CASE_1, 0xE4, 0x00, 0x00);

		rv = sc_transmit_apdu(card, &apdu);
		LOG_TEST_RET(ctx, rv, "APDU transmit failed");
		rv = sc_check_sw(card, apdu.sw1, apdu.sw2);
		LOG_TEST_RET(ctx, rv, "Delete file failed");

		if (card->cache.valid)
			sc_file_free(card->cache.current_ef);
		card->cache.current_ef = NULL;
	}

	sc_file_free(file);
	LOG_FUNC_RETURN(ctx, rv);
}
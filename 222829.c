int phar_tar_setmetadata(zval *metadata, phar_entry_info *entry, char **error TSRMLS_DC) /* {{{ */
{
	php_serialize_data_t metadata_hash;

	if (entry->metadata_str.c) {
		smart_str_free(&entry->metadata_str);
	}

	entry->metadata_str.c = 0;
	entry->metadata_str.len = 0;
	PHP_VAR_SERIALIZE_INIT(metadata_hash);
	php_var_serialize(&entry->metadata_str, &metadata, &metadata_hash TSRMLS_CC);
	PHP_VAR_SERIALIZE_DESTROY(metadata_hash);
	entry->uncompressed_filesize = entry->compressed_filesize = entry->metadata_str.len;

	if (entry->fp && entry->fp_type == PHAR_MOD) {
		php_stream_close(entry->fp);
	}

	entry->fp_type = PHAR_MOD;
	entry->is_modified = 1;
	entry->fp = php_stream_fopen_tmpfile();
	entry->offset = entry->offset_abs = 0;
	if (entry->fp == NULL) {
		spprintf(error, 0, "phar error: unable to create temporary file");
		return -1;
	}
	if (entry->metadata_str.len != php_stream_write(entry->fp, entry->metadata_str.c, entry->metadata_str.len)) {
		spprintf(error, 0, "phar tar error: unable to write metadata to magic metadata file \"%s\"", entry->filename);
		zend_hash_del(&(entry->phar->manifest), entry->filename, entry->filename_len);
		return ZEND_HASH_APPLY_STOP;
	}

	return ZEND_HASH_APPLY_KEEP;
}
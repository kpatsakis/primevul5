static zval *phar_convert_to_other(phar_archive_data *source, int convert, char *ext, php_uint32 flags TSRMLS_DC) /* {{{ */
{
	phar_archive_data *phar;
	phar_entry_info *entry, newentry;
	zval *ret;

	/* invalidate phar cache */
	PHAR_G(last_phar) = NULL;
	PHAR_G(last_phar_name) = PHAR_G(last_alias) = NULL;

	phar = (phar_archive_data *) ecalloc(1, sizeof(phar_archive_data));
	/* set whole-archive compression and type from parameter */
	phar->flags = flags;
	phar->is_data = source->is_data;

	switch (convert) {
		case PHAR_FORMAT_TAR:
			phar->is_tar = 1;
			break;
		case PHAR_FORMAT_ZIP:
			phar->is_zip = 1;
			break;
		default:
			phar->is_data = 0;
			break;
	}

	zend_hash_init(&(phar->manifest), sizeof(phar_entry_info),
		zend_get_hash_value, destroy_phar_manifest_entry, 0);
	zend_hash_init(&phar->mounted_dirs, sizeof(char *),
		zend_get_hash_value, NULL, 0);
	zend_hash_init(&phar->virtual_dirs, sizeof(char *),
		zend_get_hash_value, NULL, 0);

	phar->fp = php_stream_fopen_tmpfile();
	if (phar->fp == NULL) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "unable to create temporary file");
		return NULL;
	}
	phar->fname = source->fname;
	phar->fname_len = source->fname_len;
	phar->is_temporary_alias = source->is_temporary_alias;
	phar->alias = source->alias;

	if (source->metadata) {
		zval *t;

		t = source->metadata;
		ALLOC_ZVAL(phar->metadata);
		*phar->metadata = *t;
		zval_copy_ctor(phar->metadata);
		Z_SET_REFCOUNT_P(phar->metadata, 1);

		phar->metadata_len = 0;
	}

	/* first copy each file's uncompressed contents to a temporary file and set per-file flags */
	for (zend_hash_internal_pointer_reset(&source->manifest); SUCCESS == zend_hash_has_more_elements(&source->manifest); zend_hash_move_forward(&source->manifest)) {

		if (FAILURE == zend_hash_get_current_data(&source->manifest, (void **) &entry)) {
			zend_hash_destroy(&(phar->manifest));
			php_stream_close(phar->fp);
			efree(phar);
			zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
				"Cannot convert phar archive \"%s\"", source->fname);
			return NULL;
		}

		newentry = *entry;

		if (newentry.link) {
			newentry.link = estrdup(newentry.link);
			goto no_copy;
		}

		if (newentry.tmp) {
			newentry.tmp = estrdup(newentry.tmp);
			goto no_copy;
		}

		newentry.metadata_str.c = 0;

		if (FAILURE == phar_copy_file_contents(&newentry, phar->fp TSRMLS_CC)) {
			zend_hash_destroy(&(phar->manifest));
			php_stream_close(phar->fp);
			efree(phar);
			/* exception already thrown */
			return NULL;
		}
no_copy:
		newentry.filename = estrndup(newentry.filename, newentry.filename_len);

		if (newentry.metadata) {
			zval *t;

			t = newentry.metadata;
			ALLOC_ZVAL(newentry.metadata);
			*newentry.metadata = *t;
			zval_copy_ctor(newentry.metadata);
			Z_SET_REFCOUNT_P(newentry.metadata, 1);

			newentry.metadata_str.c = NULL;
			newentry.metadata_str.len = 0;
		}

		newentry.is_zip = phar->is_zip;
		newentry.is_tar = phar->is_tar;

		if (newentry.is_tar) {
			newentry.tar_type = (entry->is_dir ? TAR_DIR : TAR_FILE);
		}

		newentry.is_modified = 1;
		newentry.phar = phar;
		newentry.old_flags = newentry.flags & ~PHAR_ENT_COMPRESSION_MASK; /* remove compression from old_flags */
		phar_set_inode(&newentry TSRMLS_CC);
		zend_hash_add(&(phar->manifest), newentry.filename, newentry.filename_len, (void*)&newentry, sizeof(phar_entry_info), NULL);
		phar_add_virtual_dirs(phar, newentry.filename, newentry.filename_len TSRMLS_CC);
	}

	if ((ret = phar_rename_archive(phar, ext, 0 TSRMLS_CC))) {
		return ret;
	} else {
		zend_hash_destroy(&(phar->manifest));
		zend_hash_destroy(&(phar->mounted_dirs));
		zend_hash_destroy(&(phar->virtual_dirs));
		php_stream_close(phar->fp);
		efree(phar->fname);
		efree(phar);
		return NULL;
	}
}
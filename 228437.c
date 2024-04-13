PHP_METHOD(PharFileInfo, delMetadata)
{
	char *error;

	PHAR_ENTRY_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (PHAR_G(readonly) && !entry_obj->ent.entry->phar->is_data) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "Write operations disabled by the php.ini setting phar.readonly");
		return;
	}

	if (entry_obj->ent.entry->is_temp_dir) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, \
			"Phar entry is a temporary directory (not an actual entry in the archive), cannot delete metadata"); \
		return;
	}

	if (entry_obj->ent.entry->metadata) {
		if (entry_obj->ent.entry->is_persistent) {
			phar_archive_data *phar = entry_obj->ent.entry->phar;

			if (FAILURE == phar_copy_on_write(&phar TSRMLS_CC)) {
				zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar->fname);
				return;
			}
			/* re-populate after copy-on-write */
			zend_hash_find(&phar->manifest, entry_obj->ent.entry->filename, entry_obj->ent.entry->filename_len, (void **)&entry_obj->ent.entry);
		}
		zval_ptr_dtor(&entry_obj->ent.entry->metadata);
		entry_obj->ent.entry->metadata = NULL;
		entry_obj->ent.entry->is_modified = 1;
		entry_obj->ent.entry->phar->is_modified = 1;

		phar_flush(entry_obj->ent.entry->phar, 0, 0, 0, &error TSRMLS_CC);

		if (error) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
			efree(error);
			RETURN_FALSE;
		} else {
			RETURN_TRUE;
		}

	} else {
		RETURN_TRUE;
	}
}
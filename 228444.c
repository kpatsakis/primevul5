PHP_METHOD(Phar, decompressFiles)
{
	char *error;
	PHAR_ARCHIVE_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (PHAR_G(readonly) && !phar_obj->arc.archive->is_data) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Phar is readonly, cannot change compression");
		return;
	}

	if (!pharobj_cancompress(&phar_obj->arc.archive->manifest TSRMLS_CC)) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC,
			"Cannot decompress all files, some are compressed as bzip2 or gzip and cannot be decompressed");
		return;
	}

	if (phar_obj->arc.archive->is_tar) {
		RETURN_TRUE;
	} else {
		if (phar_obj->arc.archive->is_persistent && FAILURE == phar_copy_on_write(&(phar_obj->arc.archive) TSRMLS_CC)) {
			zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "phar \"%s\" is persistent, unable to copy on write", phar_obj->arc.archive->fname);
			return;
		}
		pharobj_set_compression(&phar_obj->arc.archive->manifest, PHAR_ENT_COMPRESSED_NONE TSRMLS_CC);
	}

	phar_obj->arc.archive->is_modified = 1;
	phar_flush(phar_obj->arc.archive, 0, 0, 0, &error TSRMLS_CC);

	if (error) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, "%s", error);
		efree(error);
	}

	RETURN_TRUE;
}
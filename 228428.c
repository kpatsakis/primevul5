PHP_METHOD(Phar, getMetadata)
{
	PHAR_ARCHIVE_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (phar_obj->arc.archive->metadata) {
		if (phar_obj->arc.archive->is_persistent) {
			zval *ret;
			char *buf = estrndup((char *) phar_obj->arc.archive->metadata, phar_obj->arc.archive->metadata_len);
			/* assume success, we would have failed before */
			phar_parse_metadata(&buf, &ret, phar_obj->arc.archive->metadata_len TSRMLS_CC);
			efree(buf);
			RETURN_ZVAL(ret, 0, 1);
		}
		RETURN_ZVAL(phar_obj->arc.archive->metadata, 1, 0);
	}
}
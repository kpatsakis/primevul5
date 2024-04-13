PHP_METHOD(PharFileInfo, getMetadata)
{
	PHAR_ENTRY_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (entry_obj->ent.entry->metadata) {
		if (entry_obj->ent.entry->is_persistent) {
			zval *ret;
			char *buf = estrndup((char *) entry_obj->ent.entry->metadata, entry_obj->ent.entry->metadata_len);
			/* assume success, we would have failed before */
			phar_parse_metadata(&buf, &ret, entry_obj->ent.entry->metadata_len TSRMLS_CC);
			efree(buf);
			RETURN_ZVAL(ret, 0, 1);
		}
		RETURN_ZVAL(entry_obj->ent.entry->metadata, 1, 0);
	}
}
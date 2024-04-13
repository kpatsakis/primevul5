PHP_METHOD(PharFileInfo, getCRC32)
{
	PHAR_ENTRY_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (entry_obj->ent.entry->is_dir) {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, \
			"Phar entry is a directory, does not have a CRC"); \
		return;
	}

	if (entry_obj->ent.entry->is_crc_checked) {
		RETURN_LONG(entry_obj->ent.entry->crc32);
	} else {
		zend_throw_exception_ex(spl_ce_BadMethodCallException, 0 TSRMLS_CC, \
			"Phar entry was not CRC checked"); \
	}
}
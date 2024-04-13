PHP_METHOD(Phar, stopBuffering)
{
	char *error;

	PHAR_ARCHIVE_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (PHAR_G(readonly) && !phar_obj->arc.archive->is_data) {
		zend_throw_exception_ex(spl_ce_UnexpectedValueException, 0 TSRMLS_CC,
			"Cannot write out phar archive, phar is read-only");
		return;
	}

	phar_obj->arc.archive->donotflush = 0;
	phar_flush(phar_obj->arc.archive, 0, 0, 0, &error TSRMLS_CC);

	if (error) {
		zend_throw_exception_ex(phar_ce_PharException, 0 TSRMLS_CC, "%s", error);
		efree(error);
	}
}
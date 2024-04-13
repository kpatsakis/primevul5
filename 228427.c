PHP_METHOD(Phar, isBuffering)
{
	PHAR_ARCHIVE_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(phar_obj->arc.archive->donotflush);
}
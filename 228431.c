PHP_METHOD(Phar, getVersion)
{
	PHAR_ARCHIVE_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_STRING(phar_obj->arc.archive->version, 1);
}
PHP_METHOD(Phar, getModified)
{
	PHAR_ARCHIVE_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(phar_obj->arc.archive->is_modified);
}
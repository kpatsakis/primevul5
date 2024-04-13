PHP_METHOD(Phar, isCompressed)
{
	PHAR_ARCHIVE_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (phar_obj->arc.archive->flags & PHAR_FILE_COMPRESSED_GZ) {
		RETURN_LONG(PHAR_ENT_COMPRESSED_GZ);
	}

	if (phar_obj->arc.archive->flags & PHAR_FILE_COMPRESSED_BZ2) {
		RETURN_LONG(PHAR_ENT_COMPRESSED_BZ2);
	}

	RETURN_FALSE;
}
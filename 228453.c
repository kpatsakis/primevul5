PHP_METHOD(Phar, getPath)
{
	PHAR_ARCHIVE_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_STRINGL(phar_obj->arc.archive->fname, phar_obj->arc.archive->fname_len, 1);
}
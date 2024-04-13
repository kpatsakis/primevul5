PHP_METHOD(Phar, getAlias)
{
	PHAR_ARCHIVE_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (phar_obj->arc.archive->alias && phar_obj->arc.archive->alias != phar_obj->arc.archive->fname) {
		RETURN_STRINGL(phar_obj->arc.archive->alias, phar_obj->arc.archive->alias_len, 1);
	}
}
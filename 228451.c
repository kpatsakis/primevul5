PHP_METHOD(PharFileInfo, getPharFlags)
{
	PHAR_ENTRY_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(entry_obj->ent.entry->flags & ~(PHAR_ENT_PERM_MASK|PHAR_ENT_COMPRESSION_MASK));
}
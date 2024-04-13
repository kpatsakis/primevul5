PHP_METHOD(PharFileInfo, hasMetadata)
{
	PHAR_ENTRY_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(entry_obj->ent.entry->metadata != NULL);
}
PHP_METHOD(PharFileInfo, isCRCChecked)
{
	PHAR_ENTRY_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_BOOL(entry_obj->ent.entry->is_crc_checked);
}
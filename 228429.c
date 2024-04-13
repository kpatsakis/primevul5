PHP_METHOD(Phar, getSignature)
{
	PHAR_ARCHIVE_OBJECT();
	
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	if (phar_obj->arc.archive->signature) {
		char *unknown;
		int unknown_len;

		array_init(return_value);
		add_assoc_stringl(return_value, "hash", phar_obj->arc.archive->signature, phar_obj->arc.archive->sig_len, 1);
		switch(phar_obj->arc.archive->sig_flags) {
			case PHAR_SIG_MD5:
				add_assoc_stringl(return_value, "hash_type", "MD5", 3, 1);
				break;
			case PHAR_SIG_SHA1:
				add_assoc_stringl(return_value, "hash_type", "SHA-1", 5, 1);
				break;
			case PHAR_SIG_SHA256:
				add_assoc_stringl(return_value, "hash_type", "SHA-256", 7, 1);
				break;
			case PHAR_SIG_SHA512:
				add_assoc_stringl(return_value, "hash_type", "SHA-512", 7, 1);
				break;
			case PHAR_SIG_OPENSSL:
				add_assoc_stringl(return_value, "hash_type", "OpenSSL", 7, 1);
				break;
			default:
				unknown_len = spprintf(&unknown, 0, "Unknown (%u)", phar_obj->arc.archive->sig_flags);
				add_assoc_stringl(return_value, "hash_type", unknown, unknown_len, 0);
				break;
		}
	} else {
		RETURN_FALSE;
	}
}
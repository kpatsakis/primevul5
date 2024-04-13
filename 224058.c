static void php_autoglobal_merge(HashTable *dest, HashTable *src)
{
	zval *src_entry, *dest_entry;
	zend_string *string_key;
	zend_ulong num_key;
	int globals_check = (dest == (&EG(symbol_table)));

	ZEND_HASH_FOREACH_KEY_VAL(src, num_key, string_key, src_entry) {
		if (Z_TYPE_P(src_entry) != IS_ARRAY
			|| (string_key && (dest_entry = zend_hash_find(dest, string_key)) == NULL)
			|| (string_key == NULL && (dest_entry = zend_hash_index_find(dest, num_key)) == NULL)
			|| Z_TYPE_P(dest_entry) != IS_ARRAY) {
			if (Z_REFCOUNTED_P(src_entry)) {
				Z_ADDREF_P(src_entry);
			}
			if (string_key) {
				if (!globals_check || ZSTR_LEN(string_key) != sizeof("GLOBALS") - 1
						|| memcmp(ZSTR_VAL(string_key), "GLOBALS", sizeof("GLOBALS") - 1)) {
					zend_hash_update(dest, string_key, src_entry);
				} else if (Z_REFCOUNTED_P(src_entry)) {
					Z_DELREF_P(src_entry);
				}
			} else {
				zend_hash_index_update(dest, num_key, src_entry);
			}
		} else {
			SEPARATE_ARRAY(dest_entry);
			php_autoglobal_merge(Z_ARRVAL_P(dest_entry), Z_ARRVAL_P(src_entry));
		}
	} ZEND_HASH_FOREACH_END();
}
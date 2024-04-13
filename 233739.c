long keyctl_keyring_clear(key_serial_t ringid)
{
	key_ref_t keyring_ref;
	long ret;

	keyring_ref = lookup_user_key(ringid, KEY_LOOKUP_CREATE, KEY_WRITE);
	if (IS_ERR(keyring_ref)) {
		ret = PTR_ERR(keyring_ref);
		goto error;
	}

	ret = keyring_clear(key_ref_to_ptr(keyring_ref));

	key_ref_put(keyring_ref);
error:
	return ret;

} /* end keyctl_keyring_clear() */
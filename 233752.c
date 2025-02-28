long keyctl_revoke_key(key_serial_t id)
{
	key_ref_t key_ref;
	long ret;

	key_ref = lookup_user_key(id, 0, KEY_WRITE);
	if (IS_ERR(key_ref)) {
		ret = PTR_ERR(key_ref);
		if (ret != -EACCES)
			goto error;
		key_ref = lookup_user_key(id, 0, KEY_SETATTR);
		if (IS_ERR(key_ref)) {
			ret = PTR_ERR(key_ref);
			goto error;
		}
	}

	key_revoke(key_ref_to_ptr(key_ref));
	ret = 0;

	key_ref_put(key_ref);
error:
	return ret;

} /* end keyctl_revoke_key() */
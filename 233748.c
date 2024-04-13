long keyctl_describe_key(key_serial_t keyid,
			 char __user *buffer,
			 size_t buflen)
{
	struct key *key, *instkey;
	key_ref_t key_ref;
	char *tmpbuf;
	long ret;

	key_ref = lookup_user_key(keyid, KEY_LOOKUP_PARTIAL, KEY_VIEW);
	if (IS_ERR(key_ref)) {
		/* viewing a key under construction is permitted if we have the
		 * authorisation token handy */
		if (PTR_ERR(key_ref) == -EACCES) {
			instkey = key_get_instantiation_authkey(keyid);
			if (!IS_ERR(instkey)) {
				key_put(instkey);
				key_ref = lookup_user_key(keyid,
							  KEY_LOOKUP_PARTIAL,
							  0);
				if (!IS_ERR(key_ref))
					goto okay;
			}
		}

		ret = PTR_ERR(key_ref);
		goto error;
	}

okay:
	/* calculate how much description we're going to return */
	ret = -ENOMEM;
	tmpbuf = kmalloc(PAGE_SIZE, GFP_KERNEL);
	if (!tmpbuf)
		goto error2;

	key = key_ref_to_ptr(key_ref);

	ret = snprintf(tmpbuf, PAGE_SIZE - 1,
		       "%s;%d;%d;%08x;%s",
		       key->type->name,
		       key->uid,
		       key->gid,
		       key->perm,
		       key->description ?: "");

	/* include a NUL char at the end of the data */
	if (ret > PAGE_SIZE - 1)
		ret = PAGE_SIZE - 1;
	tmpbuf[ret] = 0;
	ret++;

	/* consider returning the data */
	if (buffer && buflen > 0) {
		if (buflen > ret)
			buflen = ret;

		if (copy_to_user(buffer, tmpbuf, buflen) != 0)
			ret = -EFAULT;
	}

	kfree(tmpbuf);
error2:
	key_ref_put(key_ref);
error:
	return ret;

} /* end keyctl_describe_key() */
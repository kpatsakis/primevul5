int ecryptfs_encrypt_and_encode_filename(
	char **encoded_name,
	size_t *encoded_name_size,
	struct ecryptfs_crypt_stat *crypt_stat,
	struct ecryptfs_mount_crypt_stat *mount_crypt_stat,
	const char *name, size_t name_size)
{
	size_t encoded_name_no_prefix_size;
	int rc = 0;

	(*encoded_name) = NULL;
	(*encoded_name_size) = 0;
	if ((crypt_stat && (crypt_stat->flags & ECRYPTFS_ENCRYPT_FILENAMES))
	    || (mount_crypt_stat && (mount_crypt_stat->flags
				     & ECRYPTFS_GLOBAL_ENCRYPT_FILENAMES))) {
		struct ecryptfs_filename *filename;

		filename = kzalloc(sizeof(*filename), GFP_KERNEL);
		if (!filename) {
			printk(KERN_ERR "%s: Out of memory whilst attempting "
			       "to kzalloc [%zd] bytes\n", __func__,
			       sizeof(*filename));
			rc = -ENOMEM;
			goto out;
		}
		filename->filename = (char *)name;
		filename->filename_size = name_size;
		rc = ecryptfs_encrypt_filename(filename, crypt_stat,
					       mount_crypt_stat);
		if (rc) {
			printk(KERN_ERR "%s: Error attempting to encrypt "
			       "filename; rc = [%d]\n", __func__, rc);
			kfree(filename);
			goto out;
		}
		ecryptfs_encode_for_filename(
			NULL, &encoded_name_no_prefix_size,
			filename->encrypted_filename,
			filename->encrypted_filename_size);
		if ((crypt_stat && (crypt_stat->flags
				    & ECRYPTFS_ENCFN_USE_MOUNT_FNEK))
		    || (mount_crypt_stat
			&& (mount_crypt_stat->flags
			    & ECRYPTFS_GLOBAL_ENCFN_USE_MOUNT_FNEK)))
			(*encoded_name_size) =
				(ECRYPTFS_FNEK_ENCRYPTED_FILENAME_PREFIX_SIZE
				 + encoded_name_no_prefix_size);
		else
			(*encoded_name_size) =
				(ECRYPTFS_FEK_ENCRYPTED_FILENAME_PREFIX_SIZE
				 + encoded_name_no_prefix_size);
		(*encoded_name) = kmalloc((*encoded_name_size) + 1, GFP_KERNEL);
		if (!(*encoded_name)) {
			printk(KERN_ERR "%s: Out of memory whilst attempting "
			       "to kzalloc [%zd] bytes\n", __func__,
			       (*encoded_name_size));
			rc = -ENOMEM;
			kfree(filename->encrypted_filename);
			kfree(filename);
			goto out;
		}
		if ((crypt_stat && (crypt_stat->flags
				    & ECRYPTFS_ENCFN_USE_MOUNT_FNEK))
		    || (mount_crypt_stat
			&& (mount_crypt_stat->flags
			    & ECRYPTFS_GLOBAL_ENCFN_USE_MOUNT_FNEK))) {
			memcpy((*encoded_name),
			       ECRYPTFS_FNEK_ENCRYPTED_FILENAME_PREFIX,
			       ECRYPTFS_FNEK_ENCRYPTED_FILENAME_PREFIX_SIZE);
			ecryptfs_encode_for_filename(
			    ((*encoded_name)
			     + ECRYPTFS_FNEK_ENCRYPTED_FILENAME_PREFIX_SIZE),
			    &encoded_name_no_prefix_size,
			    filename->encrypted_filename,
			    filename->encrypted_filename_size);
			(*encoded_name_size) =
				(ECRYPTFS_FNEK_ENCRYPTED_FILENAME_PREFIX_SIZE
				 + encoded_name_no_prefix_size);
			(*encoded_name)[(*encoded_name_size)] = '\0';
			(*encoded_name_size)++;
		} else {
			rc = -ENOTSUPP;
		}
		if (rc) {
			printk(KERN_ERR "%s: Error attempting to encode "
			       "encrypted filename; rc = [%d]\n", __func__,
			       rc);
			kfree((*encoded_name));
			(*encoded_name) = NULL;
			(*encoded_name_size) = 0;
		}
		kfree(filename->encrypted_filename);
		kfree(filename);
	} else {
		rc = ecryptfs_copy_filename(encoded_name,
					    encoded_name_size,
					    name, name_size);
	}
out:
	return rc;
}
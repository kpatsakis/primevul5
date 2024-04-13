static int tipc_aead_init(struct tipc_aead **aead, struct tipc_aead_key *ukey,
			  u8 mode)
{
	struct tipc_tfm *tfm_entry, *head;
	struct crypto_aead *tfm;
	struct tipc_aead *tmp;
	int keylen, err, cpu;
	int tfm_cnt = 0;

	if (unlikely(*aead))
		return -EEXIST;

	/* Allocate a new AEAD */
	tmp = kzalloc(sizeof(*tmp), GFP_ATOMIC);
	if (unlikely(!tmp))
		return -ENOMEM;

	/* The key consists of two parts: [AES-KEY][SALT] */
	keylen = ukey->keylen - TIPC_AES_GCM_SALT_SIZE;

	/* Allocate per-cpu TFM entry pointer */
	tmp->tfm_entry = alloc_percpu(struct tipc_tfm *);
	if (!tmp->tfm_entry) {
		kfree_sensitive(tmp);
		return -ENOMEM;
	}

	/* Make a list of TFMs with the user key data */
	do {
		tfm = crypto_alloc_aead(ukey->alg_name, 0, 0);
		if (IS_ERR(tfm)) {
			err = PTR_ERR(tfm);
			break;
		}

		if (unlikely(!tfm_cnt &&
			     crypto_aead_ivsize(tfm) != TIPC_AES_GCM_IV_SIZE)) {
			crypto_free_aead(tfm);
			err = -ENOTSUPP;
			break;
		}

		err = crypto_aead_setauthsize(tfm, TIPC_AES_GCM_TAG_SIZE);
		err |= crypto_aead_setkey(tfm, ukey->key, keylen);
		if (unlikely(err)) {
			crypto_free_aead(tfm);
			break;
		}

		tfm_entry = kmalloc(sizeof(*tfm_entry), GFP_KERNEL);
		if (unlikely(!tfm_entry)) {
			crypto_free_aead(tfm);
			err = -ENOMEM;
			break;
		}
		INIT_LIST_HEAD(&tfm_entry->list);
		tfm_entry->tfm = tfm;

		/* First entry? */
		if (!tfm_cnt) {
			head = tfm_entry;
			for_each_possible_cpu(cpu) {
				*per_cpu_ptr(tmp->tfm_entry, cpu) = head;
			}
		} else {
			list_add_tail(&tfm_entry->list, &head->list);
		}

	} while (++tfm_cnt < sysctl_tipc_max_tfms);

	/* Not any TFM is allocated? */
	if (!tfm_cnt) {
		free_percpu(tmp->tfm_entry);
		kfree_sensitive(tmp);
		return err;
	}

	/* Form a hex string of some last bytes as the key's hint */
	bin2hex(tmp->hint, ukey->key + keylen - TIPC_AEAD_HINT_LEN,
		TIPC_AEAD_HINT_LEN);

	/* Initialize the other data */
	tmp->mode = mode;
	tmp->cloned = NULL;
	tmp->authsize = TIPC_AES_GCM_TAG_SIZE;
	tmp->key = kmemdup(ukey, tipc_aead_key_size(ukey), GFP_KERNEL);
	memcpy(&tmp->salt, ukey->key + keylen, TIPC_AES_GCM_SALT_SIZE);
	atomic_set(&tmp->users, 0);
	atomic64_set(&tmp->seqno, 0);
	refcount_set(&tmp->refcnt, 1);

	*aead = tmp;
	return 0;
}
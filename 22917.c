static void tipc_aead_free(struct rcu_head *rp)
{
	struct tipc_aead *aead = container_of(rp, struct tipc_aead, rcu);
	struct tipc_tfm *tfm_entry, *head, *tmp;

	if (aead->cloned) {
		tipc_aead_put(aead->cloned);
	} else {
		head = *get_cpu_ptr(aead->tfm_entry);
		put_cpu_ptr(aead->tfm_entry);
		list_for_each_entry_safe(tfm_entry, tmp, &head->list, list) {
			crypto_free_aead(tfm_entry->tfm);
			list_del(&tfm_entry->list);
			kfree(tfm_entry);
		}
		/* Free the head */
		crypto_free_aead(head->tfm);
		list_del(&head->list);
		kfree(head);
	}
	free_percpu(aead->tfm_entry);
	kfree_sensitive(aead->key);
	kfree(aead);
}
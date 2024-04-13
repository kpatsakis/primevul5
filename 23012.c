static int rtas_token_undefine(struct kvm *kvm, char *name)
{
	struct rtas_token_definition *d, *tmp;

	lockdep_assert_held(&kvm->arch.rtas_token_lock);

	list_for_each_entry_safe(d, tmp, &kvm->arch.rtas_tokens, list) {
		if (rtas_name_matches(d->handler->name, name)) {
			list_del(&d->list);
			kfree(d);
			return 0;
		}
	}

	/* It's not an error to undefine an undefined token */
	return 0;
}
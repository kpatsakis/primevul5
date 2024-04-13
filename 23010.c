static int rtas_token_define(struct kvm *kvm, char *name, u64 token)
{
	struct rtas_token_definition *d;
	struct rtas_handler *h = NULL;
	bool found;
	int i;

	lockdep_assert_held(&kvm->arch.rtas_token_lock);

	list_for_each_entry(d, &kvm->arch.rtas_tokens, list) {
		if (d->token == token)
			return -EEXIST;
	}

	found = false;
	for (i = 0; i < ARRAY_SIZE(rtas_handlers); i++) {
		h = &rtas_handlers[i];
		if (rtas_name_matches(h->name, name)) {
			found = true;
			break;
		}
	}

	if (!found)
		return -ENOENT;

	d = kzalloc(sizeof(*d), GFP_KERNEL);
	if (!d)
		return -ENOMEM;

	d->handler = h;
	d->token = token;

	list_add_tail(&d->list, &kvm->arch.rtas_tokens);

	return 0;
}
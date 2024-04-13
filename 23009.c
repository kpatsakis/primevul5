void kvmppc_rtas_tokens_free(struct kvm *kvm)
{
	struct rtas_token_definition *d, *tmp;

	list_for_each_entry_safe(d, tmp, &kvm->arch.rtas_tokens, list) {
		list_del(&d->list);
		kfree(d);
	}
}
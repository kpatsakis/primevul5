void kvm_hv_destroy_vm(struct kvm *kvm)
{
	struct kvm_hv *hv = to_kvm_hv(kvm);
	struct eventfd_ctx *eventfd;
	int i;

	idr_for_each_entry(&hv->conn_to_evt, eventfd, i)
		eventfd_ctx_put(eventfd);
	idr_destroy(&hv->conn_to_evt);
}
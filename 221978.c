void kvm_hv_init_vm(struct kvm *kvm)
{
	struct kvm_hv *hv = to_kvm_hv(kvm);

	mutex_init(&hv->hv_lock);
	idr_init(&hv->conn_to_evt);
}
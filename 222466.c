static int vm_stat_get(void *_offset, u64 *val)
{
	unsigned offset = (long)_offset;
	struct kvm *kvm;
	u64 tmp_val;

	*val = 0;
	mutex_lock(&kvm_lock);
	list_for_each_entry(kvm, &vm_list, vm_list) {
		kvm_get_stat_per_vm(kvm, offset, &tmp_val);
		*val += tmp_val;
	}
	mutex_unlock(&kvm_lock);
	return 0;
}
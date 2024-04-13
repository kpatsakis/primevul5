static int kvm_stat_data_get(void *data, u64 *val)
{
	int r = -EFAULT;
	struct kvm_stat_data *stat_data = (struct kvm_stat_data *)data;

	switch (stat_data->dbgfs_item->kind) {
	case KVM_STAT_VM:
		r = kvm_get_stat_per_vm(stat_data->kvm,
					stat_data->dbgfs_item->offset, val);
		break;
	case KVM_STAT_VCPU:
		r = kvm_get_stat_per_vcpu(stat_data->kvm,
					  stat_data->dbgfs_item->offset, val);
		break;
	}

	return r;
}
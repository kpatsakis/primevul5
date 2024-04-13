static int kvm_stat_data_clear(void *data, u64 val)
{
	int r = -EFAULT;
	struct kvm_stat_data *stat_data = (struct kvm_stat_data *)data;

	if (val)
		return -EINVAL;

	switch (stat_data->dbgfs_item->kind) {
	case KVM_STAT_VM:
		r = kvm_clear_stat_per_vm(stat_data->kvm,
					  stat_data->dbgfs_item->offset);
		break;
	case KVM_STAT_VCPU:
		r = kvm_clear_stat_per_vcpu(stat_data->kvm,
					    stat_data->dbgfs_item->offset);
		break;
	}

	return r;
}
static int kvm_s390_set_processor_feat(struct kvm *kvm,
				       struct kvm_device_attr *attr)
{
	struct kvm_s390_vm_cpu_feat data;

	if (copy_from_user(&data, (void __user *)attr->addr, sizeof(data)))
		return -EFAULT;
	if (!bitmap_subset((unsigned long *) data.feat,
			   kvm_s390_available_cpu_feat,
			   KVM_S390_VM_CPU_FEAT_NR_BITS))
		return -EINVAL;

	mutex_lock(&kvm->lock);
	if (kvm->created_vcpus) {
		mutex_unlock(&kvm->lock);
		return -EBUSY;
	}
	bitmap_copy(kvm->arch.cpu_feat, (unsigned long *) data.feat,
		    KVM_S390_VM_CPU_FEAT_NR_BITS);
	mutex_unlock(&kvm->lock);
	VM_EVENT(kvm, 3, "SET: guest feat: 0x%16.16llx.0x%16.16llx.0x%16.16llx",
			 data.feat[0],
			 data.feat[1],
			 data.feat[2]);
	return 0;
}
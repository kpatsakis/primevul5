static int kvm_s390_get_processor_feat(struct kvm *kvm,
				       struct kvm_device_attr *attr)
{
	struct kvm_s390_vm_cpu_feat data;

	bitmap_copy((unsigned long *) data.feat, kvm->arch.cpu_feat,
		    KVM_S390_VM_CPU_FEAT_NR_BITS);
	if (copy_to_user((void __user *)attr->addr, &data, sizeof(data)))
		return -EFAULT;
	VM_EVENT(kvm, 3, "GET: guest feat: 0x%16.16llx.0x%16.16llx.0x%16.16llx",
			 data.feat[0],
			 data.feat[1],
			 data.feat[2]);
	return 0;
}
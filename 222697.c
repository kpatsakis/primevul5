static int kvm_s390_vm_set_crypto(struct kvm *kvm, struct kvm_device_attr *attr)
{
	mutex_lock(&kvm->lock);
	switch (attr->attr) {
	case KVM_S390_VM_CRYPTO_ENABLE_AES_KW:
		if (!test_kvm_facility(kvm, 76)) {
			mutex_unlock(&kvm->lock);
			return -EINVAL;
		}
		get_random_bytes(
			kvm->arch.crypto.crycb->aes_wrapping_key_mask,
			sizeof(kvm->arch.crypto.crycb->aes_wrapping_key_mask));
		kvm->arch.crypto.aes_kw = 1;
		VM_EVENT(kvm, 3, "%s", "ENABLE: AES keywrapping support");
		break;
	case KVM_S390_VM_CRYPTO_ENABLE_DEA_KW:
		if (!test_kvm_facility(kvm, 76)) {
			mutex_unlock(&kvm->lock);
			return -EINVAL;
		}
		get_random_bytes(
			kvm->arch.crypto.crycb->dea_wrapping_key_mask,
			sizeof(kvm->arch.crypto.crycb->dea_wrapping_key_mask));
		kvm->arch.crypto.dea_kw = 1;
		VM_EVENT(kvm, 3, "%s", "ENABLE: DEA keywrapping support");
		break;
	case KVM_S390_VM_CRYPTO_DISABLE_AES_KW:
		if (!test_kvm_facility(kvm, 76)) {
			mutex_unlock(&kvm->lock);
			return -EINVAL;
		}
		kvm->arch.crypto.aes_kw = 0;
		memset(kvm->arch.crypto.crycb->aes_wrapping_key_mask, 0,
			sizeof(kvm->arch.crypto.crycb->aes_wrapping_key_mask));
		VM_EVENT(kvm, 3, "%s", "DISABLE: AES keywrapping support");
		break;
	case KVM_S390_VM_CRYPTO_DISABLE_DEA_KW:
		if (!test_kvm_facility(kvm, 76)) {
			mutex_unlock(&kvm->lock);
			return -EINVAL;
		}
		kvm->arch.crypto.dea_kw = 0;
		memset(kvm->arch.crypto.crycb->dea_wrapping_key_mask, 0,
			sizeof(kvm->arch.crypto.crycb->dea_wrapping_key_mask));
		VM_EVENT(kvm, 3, "%s", "DISABLE: DEA keywrapping support");
		break;
	case KVM_S390_VM_CRYPTO_ENABLE_APIE:
		if (!ap_instructions_available()) {
			mutex_unlock(&kvm->lock);
			return -EOPNOTSUPP;
		}
		kvm->arch.crypto.apie = 1;
		break;
	case KVM_S390_VM_CRYPTO_DISABLE_APIE:
		if (!ap_instructions_available()) {
			mutex_unlock(&kvm->lock);
			return -EOPNOTSUPP;
		}
		kvm->arch.crypto.apie = 0;
		break;
	default:
		mutex_unlock(&kvm->lock);
		return -ENXIO;
	}

	kvm_s390_vcpu_crypto_reset_all(kvm);
	mutex_unlock(&kvm->lock);
	return 0;
}
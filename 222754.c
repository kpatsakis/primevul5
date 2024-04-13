static int kvm_s390_cpus_from_pv(struct kvm *kvm, u16 *rcp, u16 *rrcp)
{
	struct kvm_vcpu *vcpu;
	u16 rc, rrc;
	int ret = 0;
	int i;

	/*
	 * We ignore failures and try to destroy as many CPUs as possible.
	 * At the same time we must not free the assigned resources when
	 * this fails, as the ultravisor has still access to that memory.
	 * So kvm_s390_pv_destroy_cpu can leave a "wanted" memory leak
	 * behind.
	 * We want to return the first failure rc and rrc, though.
	 */
	kvm_for_each_vcpu(i, vcpu, kvm) {
		mutex_lock(&vcpu->mutex);
		if (kvm_s390_pv_destroy_cpu(vcpu, &rc, &rrc) && !ret) {
			*rcp = rc;
			*rrcp = rrc;
			ret = -EIO;
		}
		mutex_unlock(&vcpu->mutex);
	}
	return ret;
}
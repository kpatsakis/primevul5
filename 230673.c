static void svm_vcpu_blocking(struct kvm_vcpu *vcpu)
{
	avic_set_running(vcpu, false);
}
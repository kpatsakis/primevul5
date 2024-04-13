static void avic_set_running(struct kvm_vcpu *vcpu, bool is_run)
{
	struct vcpu_svm *svm = to_svm(vcpu);

	svm->avic_is_running = is_run;
	if (is_run)
		avic_vcpu_load(vcpu, vcpu->cpu);
	else
		avic_vcpu_put(vcpu);
}
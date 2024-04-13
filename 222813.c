struct kvm_vcpu * __percpu *kvm_get_running_vcpus(void)
{
        return &kvm_running_vcpu;
}
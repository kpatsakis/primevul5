static void check_processor_compat(void *data)
{
	struct kvm_cpu_compat_check *c = data;

	*c->ret = kvm_arch_check_processor_compat(c->opaque);
}
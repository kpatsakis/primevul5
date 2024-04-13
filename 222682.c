static void kvm_resume(void)
{
	if (kvm_usage_count) {
#ifdef CONFIG_LOCKDEP
		WARN_ON(lockdep_is_held(&kvm_count_lock));
#endif
		hardware_enable_nolock(NULL);
	}
}
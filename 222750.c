int kvm_vm_create_worker_thread(struct kvm *kvm, kvm_vm_thread_fn_t thread_fn,
				uintptr_t data, const char *name,
				struct task_struct **thread_ptr)
{
	struct kvm_vm_worker_thread_context init_context = {};
	struct task_struct *thread;

	*thread_ptr = NULL;
	init_context.kvm = kvm;
	init_context.parent = current;
	init_context.thread_fn = thread_fn;
	init_context.data = data;
	init_completion(&init_context.init_done);

	thread = kthread_run(kvm_vm_worker_thread, &init_context,
			     "%s-%d", name, task_pid_nr(current));
	if (IS_ERR(thread))
		return PTR_ERR(thread);

	/* kthread_run is never supposed to return NULL */
	WARN_ON(thread == NULL);

	wait_for_completion(&init_context.init_done);

	if (!init_context.err)
		*thread_ptr = thread;

	return init_context.err;
}
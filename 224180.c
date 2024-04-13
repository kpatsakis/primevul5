SYSCALL_DEFINE4(pidfd_send_signal, int, pidfd, int, sig,
		siginfo_t __user *, info, unsigned int, flags)
{
	int ret;
	struct fd f;
	struct pid *pid;
	kernel_siginfo_t kinfo;

	/* Enforce flags be set to 0 until we add an extension. */
	if (flags)
		return -EINVAL;

	f = fdget(pidfd);
	if (!f.file)
		return -EBADF;

	/* Is this a pidfd? */
	pid = pidfd_to_pid(f.file);
	if (IS_ERR(pid)) {
		ret = PTR_ERR(pid);
		goto err;
	}

	ret = -EINVAL;
	if (!access_pidfd_pidns(pid))
		goto err;

	if (info) {
		ret = copy_siginfo_from_user_any(&kinfo, info);
		if (unlikely(ret))
			goto err;

		ret = -EINVAL;
		if (unlikely(sig != kinfo.si_signo))
			goto err;

		/* Only allow sending arbitrary signals to yourself. */
		ret = -EPERM;
		if ((task_pid(current) != pid) &&
		    (kinfo.si_code >= 0 || kinfo.si_code == SI_TKILL))
			goto err;
	} else {
		prepare_kill_siginfo(sig, &kinfo);
	}

	ret = kill_pid_info(sig, &kinfo, pid);

err:
	fdput(f);
	return ret;
}
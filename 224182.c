static int exec_mmap(struct mm_struct *mm)
{
	struct task_struct *tsk;
	struct mm_struct *old_mm, *active_mm;
	int ret;

	/* Notify parent that we're no longer interested in the old VM */
	tsk = current;
	old_mm = current->mm;
	exec_mm_release(tsk, old_mm);

	ret = mutex_lock_killable(&tsk->signal->exec_update_mutex);
	if (ret)
		return ret;

	if (old_mm) {
		sync_mm_rss(old_mm);
		/*
		 * Make sure that if there is a core dump in progress
		 * for the old mm, we get out and die instead of going
		 * through with the exec.  We must hold mmap_sem around
		 * checking core_state and changing tsk->mm.
		 */
		down_read(&old_mm->mmap_sem);
		if (unlikely(old_mm->core_state)) {
			up_read(&old_mm->mmap_sem);
			mutex_unlock(&tsk->signal->exec_update_mutex);
			return -EINTR;
		}
	}

	task_lock(tsk);
	active_mm = tsk->active_mm;
	membarrier_exec_mmap(mm);
	tsk->mm = mm;
	tsk->active_mm = mm;
	activate_mm(active_mm, mm);
	tsk->mm->vmacache_seqnum = 0;
	vmacache_flush(tsk);
	task_unlock(tsk);
	if (old_mm) {
		up_read(&old_mm->mmap_sem);
		BUG_ON(active_mm != old_mm);
		setmax_mm_hiwater_rss(&tsk->signal->maxrss, old_mm);
		mm_update_next_owner(old_mm);
		mmput(old_mm);
		return 0;
	}
	mmdrop(active_mm);
	return 0;
}
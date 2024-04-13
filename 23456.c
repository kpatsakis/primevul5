static void k_getrusage(struct task_struct *p, int who, struct rusage *r)
{
	struct task_struct *t;
	unsigned long flags;
	cputime_t utime, stime;

	memset((char *) r, 0, sizeof *r);
	utime = stime = cputime_zero;

	rcu_read_lock();
	if (!lock_task_sighand(p, &flags)) {
		rcu_read_unlock();
		return;
	}

	switch (who) {
		case RUSAGE_BOTH:
		case RUSAGE_CHILDREN:
			utime = p->signal->cutime;
			stime = p->signal->cstime;
			r->ru_nvcsw = p->signal->cnvcsw;
			r->ru_nivcsw = p->signal->cnivcsw;
			r->ru_minflt = p->signal->cmin_flt;
			r->ru_majflt = p->signal->cmaj_flt;

			if (who == RUSAGE_CHILDREN)
				break;

		case RUSAGE_SELF:
			utime = cputime_add(utime, p->signal->utime);
			stime = cputime_add(stime, p->signal->stime);
			r->ru_nvcsw += p->signal->nvcsw;
			r->ru_nivcsw += p->signal->nivcsw;
			r->ru_minflt += p->signal->min_flt;
			r->ru_majflt += p->signal->maj_flt;
			t = p;
			do {
				utime = cputime_add(utime, t->utime);
				stime = cputime_add(stime, t->stime);
				r->ru_nvcsw += t->nvcsw;
				r->ru_nivcsw += t->nivcsw;
				r->ru_minflt += t->min_flt;
				r->ru_majflt += t->maj_flt;
				t = next_thread(t);
			} while (t != p);
			break;

		default:
			BUG();
	}

	unlock_task_sighand(p, &flags);
	rcu_read_unlock();

	cputime_to_timeval(utime, &r->ru_utime);
	cputime_to_timeval(stime, &r->ru_stime);
}
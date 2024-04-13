static int lp_do_ioctl(unsigned int minor, unsigned int cmd,
	unsigned long arg, void __user *argp)
{
	int status;
	int retval = 0;

#ifdef LP_DEBUG
	printk(KERN_DEBUG "lp%d ioctl, cmd: 0x%x, arg: 0x%lx\n", minor, cmd, arg);
#endif
	if (minor >= LP_NO)
		return -ENODEV;
	if ((LP_F(minor) & LP_EXIST) == 0)
		return -ENODEV;
	switch ( cmd ) {
		case LPTIME:
			if (arg > UINT_MAX / HZ)
				return -EINVAL;
			LP_TIME(minor) = arg * HZ/100;
			break;
		case LPCHAR:
			LP_CHAR(minor) = arg;
			break;
		case LPABORT:
			if (arg)
				LP_F(minor) |= LP_ABORT;
			else
				LP_F(minor) &= ~LP_ABORT;
			break;
		case LPABORTOPEN:
			if (arg)
				LP_F(minor) |= LP_ABORTOPEN;
			else
				LP_F(minor) &= ~LP_ABORTOPEN;
			break;
		case LPCAREFUL:
			if (arg)
				LP_F(minor) |= LP_CAREFUL;
			else
				LP_F(minor) &= ~LP_CAREFUL;
			break;
		case LPWAIT:
			LP_WAIT(minor) = arg;
			break;
		case LPSETIRQ: 
			return -EINVAL;
			break;
		case LPGETIRQ:
			if (copy_to_user(argp, &LP_IRQ(minor),
					sizeof(int)))
				return -EFAULT;
			break;
		case LPGETSTATUS:
			if (mutex_lock_interruptible(&lp_table[minor].port_mutex))
				return -EINTR;
			lp_claim_parport_or_block (&lp_table[minor]);
			status = r_str(minor);
			lp_release_parport (&lp_table[minor]);
			mutex_unlock(&lp_table[minor].port_mutex);

			if (copy_to_user(argp, &status, sizeof(int)))
				return -EFAULT;
			break;
		case LPRESET:
			lp_reset(minor);
			break;
#ifdef LP_STATS
		case LPGETSTATS:
			if (copy_to_user(argp, &LP_STAT(minor),
					sizeof(struct lp_stats)))
				return -EFAULT;
			if (capable(CAP_SYS_ADMIN))
				memset(&LP_STAT(minor), 0,
						sizeof(struct lp_stats));
			break;
#endif
 		case LPGETFLAGS:
 			status = LP_F(minor);
			if (copy_to_user(argp, &status, sizeof(int)))
				return -EFAULT;
			break;

		default:
			retval = -EINVAL;
	}
	return retval;
}
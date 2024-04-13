static int input_add_uevent_bm_var(struct kobj_uevent_env *env,
				   const char *name, unsigned long *bitmap, int max)
{
	int len;

	if (add_uevent_var(env, "%s", name))
		return -ENOMEM;

	len = input_print_bitmap(&env->buf[env->buflen - 1],
				 sizeof(env->buf) - env->buflen,
				 bitmap, max, false);
	if (len >= (sizeof(env->buf) - env->buflen))
		return -ENOMEM;

	env->buflen += len;
	return 0;
}
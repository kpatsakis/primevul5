static int input_add_uevent_modalias_var(struct kobj_uevent_env *env,
					 struct input_dev *dev)
{
	int len;

	if (add_uevent_var(env, "MODALIAS="))
		return -ENOMEM;

	len = input_print_modalias(&env->buf[env->buflen - 1],
				   sizeof(env->buf) - env->buflen,
				   dev, 0);
	if (len >= (sizeof(env->buf) - env->buflen))
		return -ENOMEM;

	env->buflen += len;
	return 0;
}
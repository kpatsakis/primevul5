static int trace_set_options(struct trace_array *tr, char *option)
{
	char *cmp;
	int neg = 0;
	int ret = -ENODEV;
	int i;
	size_t orig_len = strlen(option);

	cmp = strstrip(option);

	if (strncmp(cmp, "no", 2) == 0) {
		neg = 1;
		cmp += 2;
	}

	mutex_lock(&trace_types_lock);

	for (i = 0; trace_options[i]; i++) {
		if (strcmp(cmp, trace_options[i]) == 0) {
			ret = set_tracer_flag(tr, 1 << i, !neg);
			break;
		}
	}

	/* If no option could be set, test the specific tracer options */
	if (!trace_options[i])
		ret = set_tracer_option(tr, cmp, neg);

	mutex_unlock(&trace_types_lock);

	/*
	 * If the first trailing whitespace is replaced with '\0' by strstrip,
	 * turn it back into a space.
	 */
	if (orig_len > strlen(option))
		option[strlen(option)] = ' ';

	return ret;
}
static ssize_t dlpar_store(struct class *class, struct class_attribute *attr,
			   const char *buf, size_t count)
{
	struct pseries_hp_errorlog hp_elog;
	char *argbuf;
	char *args;
	int rc;

	args = argbuf = kstrdup(buf, GFP_KERNEL);
	if (!argbuf) {
		pr_info("Could not allocate resources for DLPAR operation\n");
		kfree(argbuf);
		return -ENOMEM;
	}

	/*
	 * Parse out the request from the user, this will be in the form:
	 * <resource> <action> <id_type> <id>
	 */
	rc = dlpar_parse_resource(&args, &hp_elog);
	if (rc)
		goto dlpar_store_out;

	rc = dlpar_parse_action(&args, &hp_elog);
	if (rc)
		goto dlpar_store_out;

	rc = dlpar_parse_id_type(&args, &hp_elog);
	if (rc)
		goto dlpar_store_out;

	rc = handle_dlpar_errorlog(&hp_elog);

dlpar_store_out:
	kfree(argbuf);

	if (rc)
		pr_err("Could not handle DLPAR request \"%s\"\n", buf);

	return rc ? rc : count;
}
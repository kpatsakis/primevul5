static int err_attr_cb(const struct nlattr *attr, void *data)
{
	const struct nlattr **tb = data;
	uint16_t type;

	if (mnl_attr_type_valid(attr, NLMSGERR_ATTR_MAX) < 0) {
		fprintf(stderr, "Invalid extack attribute\n");
		return MNL_CB_ERROR;
	}

	type = mnl_attr_get_type(attr);
	if (mnl_attr_validate(attr, extack_policy[type]) < 0) {
		fprintf(stderr, "extack attribute %d failed validation\n",
			type);
		return MNL_CB_ERROR;
	}

	tb[type] = attr;
	return MNL_CB_OK;
}
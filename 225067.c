static struct device_node *dlpar_parse_cc_node(struct cc_workarea *ccwa)
{
	struct device_node *dn;
	const char *name;

	dn = kzalloc(sizeof(*dn), GFP_KERNEL);
	if (!dn)
		return NULL;

	name = (const char *)ccwa + be32_to_cpu(ccwa->name_offset);
	dn->full_name = kstrdup(name, GFP_KERNEL);
	if (!dn->full_name) {
		kfree(dn);
		return NULL;
	}

	of_node_set_flag(dn, OF_DYNAMIC);
	of_node_init(dn);

	return dn;
}
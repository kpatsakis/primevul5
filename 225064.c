int dlpar_attach_node(struct device_node *dn, struct device_node *parent)
{
	int rc;

	dn->parent = parent;

	rc = of_attach_node(dn);
	if (rc) {
		printk(KERN_ERR "Failed to add device node %pOF\n", dn);
		return rc;
	}

	return 0;
}
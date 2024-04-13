nfs4_label_release_security(struct nfs4_label *label)
{
	if (label)
		security_release_secctx(label->label, label->len);
}
static int tipc_nl_retrieve_key(struct nlattr **attrs,
				struct tipc_aead_key **pkey)
{
	struct nlattr *attr = attrs[TIPC_NLA_NODE_KEY];
	struct tipc_aead_key *key;

	if (!attr)
		return -ENODATA;

	if (nla_len(attr) < sizeof(*key))
		return -EINVAL;
	key = (struct tipc_aead_key *)nla_data(attr);
	if (key->keylen > TIPC_AEAD_KEYLEN_MAX ||
	    nla_len(attr) < tipc_aead_key_size(key))
		return -EINVAL;

	*pkey = key;
	return 0;
}
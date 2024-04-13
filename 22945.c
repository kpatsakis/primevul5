static char *tipc_key_change_dump(struct tipc_key old, struct tipc_key new,
				  char *buf)
{
	struct tipc_key *key = &old;
	int k, i = 0;
	char *s;

	/* Output format: "[%s %s %s] -> [%s %s %s]", max len = 32 */
again:
	i += scnprintf(buf + i, 32 - i, "[");
	for (k = KEY_1; k <= KEY_3; k++) {
		if (k == key->passive)
			s = "pas";
		else if (k == key->active)
			s = "act";
		else if (k == key->pending)
			s = "pen";
		else
			s = "-";
		i += scnprintf(buf + i, 32 - i,
			       (k != KEY_3) ? "%s " : "%s", s);
	}
	if (key != &new) {
		i += scnprintf(buf + i, 32 - i, "] -> ");
		key = &new;
		goto again;
	}
	i += scnprintf(buf + i, 32 - i, "]");
	return buf;
}
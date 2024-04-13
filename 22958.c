static inline void tipc_crypto_key_set_state(struct tipc_crypto *c,
					     u8 new_passive,
					     u8 new_active,
					     u8 new_pending)
{
	struct tipc_key old = c->key;
	char buf[32];

	c->key.keys = ((new_passive & KEY_MASK) << (KEY_BITS * 2)) |
		      ((new_active  & KEY_MASK) << (KEY_BITS)) |
		      ((new_pending & KEY_MASK));

	pr_debug("%s: key changing %s ::%pS\n", c->name,
		 tipc_key_change_dump(old, c->key, buf),
		 __builtin_return_address(0));
}
static int samldb_generate_sAMAccountName(struct ldb_context *ldb,
					  struct ldb_message *msg)
{
	char *name;

	/* Format: $000000-000000000000 */

	name = talloc_asprintf(msg, "$%.6X-%.6X%.6X",
				(unsigned int)generate_random(),
				(unsigned int)generate_random(),
				(unsigned int)generate_random());
	if (name == NULL) {
		return ldb_oom(ldb);
	}
	return ldb_msg_add_steal_string(msg, "sAMAccountName", name);
}

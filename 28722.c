static int samldb_prim_group_trigger(struct samldb_ctx *ac)
{
	int ret;

	if (ac->req->operation == LDB_ADD) {
		ret = samldb_prim_group_set(ac);
	} else {
		ret = samldb_prim_group_change(ac);
	}

	return ret;
}

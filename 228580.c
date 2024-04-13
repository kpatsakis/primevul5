int cil_reset_ast(struct cil_tree_node *current)
{
	int rc = SEPOL_ERR;

	rc = cil_tree_walk(current, __cil_reset_node, NULL, NULL, NULL);
	if (rc != SEPOL_OK) {
		cil_log(CIL_ERR, "Failed to reset AST\n");
		return SEPOL_ERR;
	}

	return SEPOL_OK;
}
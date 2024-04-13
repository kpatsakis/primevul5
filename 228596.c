static void cil_reset_userattr(struct cil_userattribute *attr)
{
	struct cil_list_item *expr = NULL;
	struct cil_list_item *next = NULL;

	/* during a re-resolve, we need to reset the lists of expression stacks associated with this attribute from a userattribute statement */
	if (attr->expr_list != NULL) {
		/* we don't want to destroy the expression stacks (cil_list) inside
		 * this list cil_list_destroy destroys sublists, so we need to do it
		 * manually */
		expr = attr->expr_list->head;
		while (expr != NULL) {
			next = expr->next;
			cil_list_item_destroy(&expr, CIL_FALSE);
			expr = next;
		}
		free(attr->expr_list);
		attr->expr_list = NULL;
	}
}
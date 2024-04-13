static void cil_reset_typeattr(struct cil_typeattribute *attr)
{
	/* during a re-resolve, we need to reset the lists of expression stacks  associated with this attribute from a attributetypes statement */
	if (attr->expr_list != NULL) {
		/* we don't want to destroy the expression stacks (cil_list) inside
		 * this list cil_list_destroy destroys sublists, so we need to do it
		 * manually */
		struct cil_list_item *expr = attr->expr_list->head;
		while (expr != NULL) {
			struct cil_list_item *next = expr->next;
			cil_list_item_destroy(&expr, CIL_FALSE);
			expr = next;
		}
		free(attr->expr_list);
		attr->expr_list = NULL;
	}
	attr->used = CIL_FALSE;
	attr->keep = CIL_FALSE;
}
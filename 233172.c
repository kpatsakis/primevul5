spell_print_tree(wordnode_T *root)
{
    if (root != NULL)
    {
	/* Clear the "wn_u1.index" fields, used to remember what has been
	 * done. */
	spell_clear_flags(root);

	/* Recursively print the tree. */
	spell_print_node(root, 0);
    }
}
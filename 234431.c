lp_use_properties(struct lp_style_type *lp, int tag)
{
    /*  This function looks for a linestyle defined by 'tag' and copies
     *  its data into the structure 'lp'.
     */

    struct linestyle_def *this;
    int save_flags = lp->flags;

    this = first_linestyle;
    while (this != NULL) {
	if (this->tag == tag) {
	    *lp = this->lp_properties;
	    lp->flags = save_flags;
	    return;
	} else {
	    this = this->next;
	}
    }

    /* No user-defined style with this tag; fall back to default line type. */
    load_linetype(lp, tag);
}
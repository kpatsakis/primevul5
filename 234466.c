load_linetype(struct lp_style_type *lp, int tag)
{
    struct linestyle_def *this;
    TBOOLEAN recycled = FALSE;

recycle:

    if ((tag > 0)
    && (monochrome || (term && (term->flags & TERM_MONOCHROME)))) {
	for (this = first_mono_linestyle; this; this = this->next) {
	    if (tag == this->tag) {
		*lp = this->lp_properties;
		return;
	    }
	}

	/* This linetype wasn't defined explicitly.		*/
	/* Should we recycle one of the first N linetypes?	*/
	if (tag > mono_recycle_count && mono_recycle_count > 0) {
	    tag = (tag-1) % mono_recycle_count + 1;
	    goto recycle;
	}

	return;
    }

    this = first_perm_linestyle;
    while (this != NULL) {
	if (this->tag == tag) {
	    /* Always load color, width, and dash properties */
	    lp->l_type = this->lp_properties.l_type;
	    lp->l_width = this->lp_properties.l_width;
	    lp->pm3d_color = this->lp_properties.pm3d_color;
	    lp->d_type = this->lp_properties.d_type;
	    lp->custom_dash_pattern = this->lp_properties.custom_dash_pattern;

	    /* Needed in version 5.0 to handle old terminals (pbm hpgl ...) */
	    /* with no support for user-specified colors */
	    if (term && term->set_color == null_set_color)
		lp->l_type = tag;

	    /* Do not recycle point properties. */
	    /* FIXME: there should be a separate command "set pointtype cycle N" */
	    if (!recycled) {
	    	lp->p_type = this->lp_properties.p_type;
	    	lp->p_interval = this->lp_properties.p_interval;
	    	lp->p_size = this->lp_properties.p_size;
	    	memcpy(lp->p_char, this->lp_properties.p_char, sizeof(lp->p_char));
	    }
	    return;
	} else {
	    this = this->next;
	}
    }

    /* This linetype wasn't defined explicitly.		*/
    /* Should we recycle one of the first N linetypes?	*/
    if (tag > linetype_recycle_count && linetype_recycle_count > 0) {
	tag = (tag-1) % linetype_recycle_count + 1;
	recycled = TRUE;
	goto recycle;
    }

    /* No user-defined linetype with this tag; fall back to default line type. */
    /* NB: We assume that the remaining fields of lp have been initialized. */
    lp->l_type = tag - 1;
    lp->pm3d_color.type = TC_LT;
    lp->pm3d_color.lt = lp->l_type;
    lp->d_type = DASHTYPE_SOLID;
    lp->p_type = (tag <= 0) ? -1 : tag - 1;
}
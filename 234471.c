init_monochrome()
{
    struct lp_style_type mono_default[] = DEFAULT_MONO_LINETYPES;

    if (first_mono_linestyle == NULL) {
	int i, n = sizeof(mono_default) / sizeof(struct lp_style_type);
	struct linestyle_def *new;
	/* copy default list into active list */
	for (i=n; i>0; i--) {
	    new = gp_alloc(sizeof(struct linestyle_def), NULL);
	    new->next = first_mono_linestyle;
	    new->lp_properties = mono_default[i-1];
	    new->tag = i;
	    first_mono_linestyle = new;
	}
    }
}
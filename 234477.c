load_dashtype(struct t_dashtype *dt, int tag)
{
    struct custom_dashtype_def *this;
    struct t_dashtype loc_dt = DEFAULT_DASHPATTERN;

    this = first_custom_dashtype;
    while (this != NULL) {
	if (this->tag == tag) {
	    *dt = this->dashtype;
	    memcpy(dt->dstring, this->dashtype.dstring, sizeof(dt->dstring));
	    return this->d_type;
	} else {
	    this = this->next;
	}
    }

    /* not found, fall back to default, terminal-dependent dashtype */
    *dt = loc_dt;
    return tag - 1;
}
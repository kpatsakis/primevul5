static inline void sca_copy_entry(struct esca_entry *d, struct bsca_entry *s)
{
	d->sda = s->sda;
	d->sigp_ctrl.c = s->sigp_ctrl.c;
	d->sigp_ctrl.scn = s->sigp_ctrl.scn;
}
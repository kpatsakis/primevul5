void agmethod_init(Agraph_t * g, void *obj)
{
    if (g->clos->callbacks_enabled)
	aginitcb(g, obj, g->clos->cb);
    else
	agrecord_callback(g, obj, CB_INITIALIZE, NILsym);
}
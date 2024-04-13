void agmethod_upd(Agraph_t * g, void *obj, Agsym_t * sym)
{
    if (g->clos->callbacks_enabled)
	agupdcb(g, obj, sym, g->clos->cb);
    else
	agrecord_callback(g, obj, CB_UPDATE, sym);
}
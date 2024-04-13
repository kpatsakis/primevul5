void agmethod_delete(Agraph_t * g, void *obj)
{
    if (g->clos->callbacks_enabled)
	agdelcb(g, obj, g->clos->cb);
    else
	agrecord_callback(g, obj, CB_DELETION, NILsym);
}
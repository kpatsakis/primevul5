void msg_free(message_data_t *m)
{
    if (m->data)
	prot_free(m->data);
    if (m->f)
	fclose(m->f);
    free(m->id);
    free(m->path);
    free(m->control);
    free(m->date);

    strarray_fini(&m->rcpt);

    spool_free_hdrcache(m->hdrcache);

    free(m);
}
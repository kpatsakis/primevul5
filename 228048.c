free_typebuf(void)
{
    if (typebuf.tb_buf == typebuf_init)
	internal_error("Free typebuf 1");
    else
	VIM_CLEAR(typebuf.tb_buf);
    if (typebuf.tb_noremap == noremapbuf_init)
	internal_error("Free typebuf 2");
    else
	VIM_CLEAR(typebuf.tb_noremap);
}
save_typebuf(void)
{
    init_typebuf();
    saved_typebuf[curscript] = typebuf;
    // If out of memory: restore typebuf and close file.
    if (alloc_typebuf() == FAIL)
    {
	closescript();
	return FAIL;
    }
    return OK;
}
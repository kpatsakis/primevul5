restore_typeahead(tasave_T *tp, int overwrite UNUSED)
{
    if (tp->typebuf_valid)
    {
	free_typebuf();
	typebuf = tp->save_typebuf;
    }

    old_char = tp->old_char;
    old_mod_mask = tp->old_mod_mask;

    free_buff(&readbuf1);
    readbuf1 = tp->save_readbuf1;
    free_buff(&readbuf2);
    readbuf2 = tp->save_readbuf2;
# ifdef USE_INPUT_BUF
    set_input_buf(tp->save_inputbuf, overwrite);
# endif
}
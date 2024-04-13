typebuf_changed(
    int		tb_change_cnt)	// old value of typebuf.tb_change_cnt
{
    return (tb_change_cnt != 0 && (typebuf.tb_change_cnt != tb_change_cnt
#if defined(FEAT_CLIENTSERVER) || defined(FEAT_EVAL)
	    || typebuf_was_filled
#endif
	   ));
}
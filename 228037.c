at_ctrl_x_key(void)
{
    char_u  *p = typebuf.tb_buf + typebuf.tb_off;
    int	    c = *p;

    if (typebuf.tb_len > 3
	    && c == K_SPECIAL
	    && p[1] == KS_MODIFIER
	    && (p[2] & MOD_MASK_CTRL))
	c = p[3] & 0x1f;
    return vim_is_ctrl_x_key(c);
}
null_set_color(struct t_colorspec *colorspec)
{
    if (colorspec->type == TC_LT)
	term->linetype(colorspec->lt);
}
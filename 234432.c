on_page(int x, int y)
{
    if (term->flags & TERM_CAN_CLIP)
	return TRUE;

    if ((0 < x && x < term->xmax) && (0 < y && y < term->ymax))
	return TRUE;

    return FALSE;
}
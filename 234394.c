clear_command()
{

    term_start_plot();

    if (multiplot && term->fillbox) {
	int xx1 = xoffset * term->xmax;
	int yy1 = yoffset * term->ymax;
	unsigned int width = xsize * term->xmax;
	unsigned int height = ysize * term->ymax;
	(*term->fillbox) (0, xx1, yy1, width, height);
    }
    term_end_plot();

    screen_ok = FALSE;
    c_token++;

}
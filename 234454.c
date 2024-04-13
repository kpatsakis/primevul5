parse_term_size( float *xsize, float *ysize, size_units default_units )
{
    size_units units = default_units;

    if (END_OF_COMMAND)
	int_error(c_token, "size requires two numbers:  xsize, ysize");
    *xsize = real_expression();
    if (almost_equals(c_token,"in$ches")) {
	c_token++;
	units = INCHES;
    } else if (equals(c_token,"cm")) {
	c_token++;
	units = CM;
    }
    switch (units) {
    case INCHES:	*xsize *= gp_resolution; break;
    case CM:		*xsize *= (float)gp_resolution / 2.54; break;
    case PIXELS:
    default:		 break;
    }

    if (!equals(c_token++,","))
	int_error(c_token, "size requires two numbers:  xsize, ysize");
    *ysize = real_expression();
    if (almost_equals(c_token,"in$ches")) {
	c_token++;
	units = INCHES;
    } else if (equals(c_token,"cm")) {
	c_token++;
	units = CM;
    }
    switch (units) {
    case INCHES:	*ysize *= gp_resolution; break;
    case CM:		*ysize *= (float)gp_resolution / 2.54; break;
    case PIXELS:
    default:		 break;
    }

    if (*xsize < 1 || *ysize < 1)
	int_error(c_token, "size: out of range");

    return units;
}
 */
static void
xmlXPathCompNumber(xmlXPathParserContextPtr ctxt)
{
    double ret = 0.0;
    int ok = 0;
    int exponent = 0;
    int is_exponent_negative = 0;
#ifdef __GNUC__
    unsigned long tmp = 0;
    double temp;
#endif

    CHECK_ERROR;
    if ((CUR != '.') && ((CUR < '0') || (CUR > '9'))) {
        XP_ERROR(XPATH_NUMBER_ERROR);
    }
#ifdef __GNUC__
    /*
     * tmp/temp is a workaround against a gcc compiler bug
     * http://veillard.com/gcc.bug
     */
    ret = 0;
    while ((CUR >= '0') && (CUR <= '9')) {
	ret = ret * 10;
	tmp = (CUR - '0');
        ok = 1;
        NEXT;
	temp = (double) tmp;
	ret = ret + temp;
    }
#else
    ret = 0;
    while ((CUR >= '0') && (CUR <= '9')) {
	ret = ret * 10 + (CUR - '0');
	ok = 1;
	NEXT;
    }
#endif
    if (CUR == '.') {
	int v, frac = 0, max;
	double fraction = 0;

        NEXT;
        if (((CUR < '0') || (CUR > '9')) && (!ok)) {
            XP_ERROR(XPATH_NUMBER_ERROR);
        }
        while (CUR == '0') {
            frac = frac + 1;
            NEXT;
        }
        max = frac + MAX_FRAC;
        while ((CUR >= '0') && (CUR <= '9') && (frac < max)) {
	    v = (CUR - '0');
	    fraction = fraction * 10 + v;
	    frac = frac + 1;
            NEXT;
        }
        fraction /= pow(10.0, frac);
        ret = ret + fraction;
        while ((CUR >= '0') && (CUR <= '9'))
            NEXT;
    }
    if ((CUR == 'e') || (CUR == 'E')) {
        NEXT;
        if (CUR == '-') {
            is_exponent_negative = 1;
            NEXT;
        } else if (CUR == '+') {
	    NEXT;
	}
        while ((CUR >= '0') && (CUR <= '9')) {
            if (exponent < 1000000)
                exponent = exponent * 10 + (CUR - '0');
            NEXT;
        }
        if (is_exponent_negative)
            exponent = -exponent;
        ret *= pow(10.0, (double) exponent);
    }
    PUSH_LONG_EXPR(XPATH_OP_VALUE, XPATH_NUMBER, 0, 0,
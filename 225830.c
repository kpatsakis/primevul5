 */
void
xmlXPathRoundFunction(xmlXPathParserContextPtr ctxt, int nargs) {
    double f;

    CHECK_ARITY(1);
    CAST_TO_NUMBER;
    CHECK_TYPE(XPATH_NUMBER);

    f = ctxt->value->floatval;

    /* Test for zero to keep negative zero unchanged. */
    if ((xmlXPathIsNaN(f)) || (f == 0.0))
	return;

    if ((f >= -0.5) && (f < 0.0)) {
        /* Negative zero. */
        ctxt->value->floatval = xmlXPathNZERO;
    }
    else {
        double rounded = floor(f);
        if (f - rounded >= 0.5)
            rounded += 1.0;
        ctxt->value->floatval = rounded;
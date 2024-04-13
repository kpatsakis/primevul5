
static int
xmlXPathIsPositionalPredicate(xmlXPathParserContextPtr ctxt,
			    xmlXPathStepOpPtr op,
			    int *maxPos)
{

    xmlXPathStepOpPtr exprOp;

    /*
    * BIG NOTE: This is not intended for XPATH_OP_FILTER yet!
    */

    /*
    * If not -1, then ch1 will point to:
    * 1) For predicates (XPATH_OP_PREDICATE):
    *    - an inner predicate operator
    * 2) For filters (XPATH_OP_FILTER):
    *    - an inner filter operater OR
    *    - an expression selecting the node set.
    *      E.g. "key('a', 'b')" or "(//foo | //bar)".
    */
    if ((op->op != XPATH_OP_PREDICATE) && (op->op != XPATH_OP_FILTER))
	return(0);

    if (op->ch2 != -1) {
	exprOp = &ctxt->comp->steps[op->ch2];
    } else
	return(0);

    if ((exprOp != NULL) &&
	(exprOp->op == XPATH_OP_VALUE) &&
	(exprOp->value4 != NULL) &&
	(((xmlXPathObjectPtr) exprOp->value4)->type == XPATH_NUMBER))
    {
        double floatval = ((xmlXPathObjectPtr) exprOp->value4)->floatval;

	/*
	* We have a "[n]" predicate here.
	* TODO: Unfortunately this simplistic test here is not
	* able to detect a position() predicate in compound
	* expressions like "[@attr = 'a" and position() = 1],
	* and even not the usage of position() in
	* "[position() = 1]"; thus - obviously - a position-range,
	* like it "[position() < 5]", is also not detected.
	* Maybe we could rewrite the AST to ease the optimization.
	*/

        if ((floatval > INT_MIN) && (floatval < INT_MAX)) {
	    *maxPos = (int) floatval;
            if (floatval == (double) *maxPos)
                return(1);
        }
    }
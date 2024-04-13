xmlXPathCompExprAdd(xmlXPathCompExprPtr comp, int ch1, int ch2,
   xmlXPathOp op, int value,
   int value2, int value3, void *value4, void *value5) {
    if (comp->nbStep >= comp->maxStep) {
	xmlXPathStepOp *real;

        if (comp->maxStep >= XPATH_MAX_STEPS) {
	    xmlXPathErrMemory(NULL, "adding step\n");
	    return(-1);
        }
	comp->maxStep *= 2;
	real = (xmlXPathStepOp *) xmlRealloc(comp->steps,
		                      comp->maxStep * sizeof(xmlXPathStepOp));
	if (real == NULL) {
	    comp->maxStep /= 2;
	    xmlXPathErrMemory(NULL, "adding step\n");
	    return(-1);
	}
	comp->steps = real;
    }
    comp->last = comp->nbStep;
    comp->steps[comp->nbStep].ch1 = ch1;
    comp->steps[comp->nbStep].ch2 = ch2;
    comp->steps[comp->nbStep].op = op;
    comp->steps[comp->nbStep].value = value;
    comp->steps[comp->nbStep].value2 = value2;
    comp->steps[comp->nbStep].value3 = value3;
    if ((comp->dict != NULL) &&
        ((op == XPATH_OP_FUNCTION) || (op == XPATH_OP_VARIABLE) ||
	 (op == XPATH_OP_COLLECT))) {
        if (value4 != NULL) {
	    comp->steps[comp->nbStep].value4 = (xmlChar *)
	        (void *)xmlDictLookup(comp->dict, value4, -1);
	    xmlFree(value4);
	} else
	    comp->steps[comp->nbStep].value4 = NULL;
        if (value5 != NULL) {
	    comp->steps[comp->nbStep].value5 = (xmlChar *)
	        (void *)xmlDictLookup(comp->dict, value5, -1);
	    xmlFree(value5);
	} else
	    comp->steps[comp->nbStep].value5 = NULL;
    } else {
	comp->steps[comp->nbStep].value4 = value4;
	comp->steps[comp->nbStep].value5 = value5;
    }
    comp->steps[comp->nbStep].cache = NULL;
    return(comp->nbStep++);
}
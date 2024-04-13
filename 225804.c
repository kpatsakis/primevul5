xmlXPathDebugDumpCompExpr(FILE *output, xmlXPathCompExprPtr comp,
	                  int depth) {
    int i;
    char shift[100];

    if ((output == NULL) || (comp == NULL)) return;

    for (i = 0;((i < depth) && (i < 25));i++)
        shift[2 * i] = shift[2 * i + 1] = ' ';
    shift[2 * i] = shift[2 * i + 1] = 0;

    fprintf(output, "%s", shift);

#ifdef XPATH_STREAMING
    if (comp->stream) {
        fprintf(output, "Streaming Expression\n");
    } else
#endif
    {
        fprintf(output, "Compiled Expression : %d elements\n",
                comp->nbStep);
        i = comp->last;
        xmlXPathDebugDumpStepOp(output, comp, &comp->steps[i], depth + 1);
    }
}
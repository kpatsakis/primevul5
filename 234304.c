static Agraph_t *pop_subg(void)
{
    Agraph_t *g;
    if (GSP == 0) {
	fprintf(stderr, "graphml2gv: Gstack underflow in graph parser\n");
	exit(1);
    }
    g = Gstack[--GSP];
    if (GSP > 0)
	G = Gstack[GSP - 1];
    return g;
}
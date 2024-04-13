static void push_subg(Agraph_t * g)
{
    if (GSP == STACK_DEPTH) {
	fprintf(stderr, "graphml2gv: Too many (> %d) nestings of subgraphs\n",
		STACK_DEPTH);
	exit(1);
    } else if (GSP == 0)
	root = g;
    G = Gstack[GSP++] = g;
}
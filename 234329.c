int main(int argc, char **argv)
{
    Agraph_t *G;
    Agraph_t *prev = 0;
    FILE *inFile;
    int rv, gcnt = 0;

#ifdef HAVE_EXPAT
    initargs(argc, argv);
    while ((inFile = getFile())) {
	while ((G = graphml_to_gv(nameOf(gname, gcnt), inFile, &rv))) {
	    gcnt++;
	    if (prev)
		agclose(prev);
	    prev = G;
	    if (Verbose) 
		fprintf (stderr, "%s: %d nodes %d edges\n",
		    agnameof (G), agnnodes(G), agnedges(G));
	    agwrite(G, outFile);
	    fflush(outFile);
	}
    }
    exit(rv);
#else
    fputs("cvtgxl: not configured for conversion from GXL to GV\n", stderr);
    exit(1);
#endif
}
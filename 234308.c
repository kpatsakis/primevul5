static void setAttr(char *name, char *value, userdata_t * ud)
{
    switch (Current_class) {
    case TAG_GRAPH:
	setGraphAttr(G, name, value, ud);
	break;
    case TAG_NODE:
	setNodeAttr(N, name, value, ud);
	break;
    case TAG_EDGE:
	setEdgeAttr(E, name, value, ud);
	break;
    }
}
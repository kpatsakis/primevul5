static Agedge_t *bind_edge(const char *tail, const char *head)
{
    Agnode_t *tailNode, *headNode;
    char *key = 0;

    tailNode = agnode(G, (char *) tail, 1);
    headNode = agnode(G, (char *) head, 1);
    E = agedge(G, tailNode, headNode, key, 1);
    return E;
}
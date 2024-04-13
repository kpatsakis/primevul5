static Agnode_t *bind_node(const char *name)
{
    N = agnode(G, (char *) name, 1);
    return N;
}
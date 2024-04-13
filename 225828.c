xmlXPathNodeSetClear(xmlNodeSetPtr set, int hasNsNodes)
{
    xmlXPathNodeSetClearFromPos(set, 0, hasNsNodes);
}
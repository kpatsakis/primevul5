    static int wrap_cmp( xmlNodePtr x, xmlNodePtr y )
    {
        int res = xmlXPathCmpNodesExt(x, y);
        return res == -2 ? res : -res;
    }
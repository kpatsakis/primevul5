xmlXPathNewNodeSetList(xmlNodeSetPtr val)
{
    xmlXPathObjectPtr ret;
    int i;

    if (val == NULL)
        ret = NULL;
    else if (val->nodeTab == NULL)
        ret = xmlXPathNewNodeSet(NULL);
    else {
        ret = xmlXPathNewNodeSet(val->nodeTab[0]);
        if (ret) {
            for (i = 1; i < val->nodeNr; ++i) {
                if (xmlXPathNodeSetAddUnique(ret->nodesetval, val->nodeTab[i])
		    < 0) break;
	    }
	}
    }

    return (ret);
}
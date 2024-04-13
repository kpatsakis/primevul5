skipString(const xmlChar *cur, int end) {
    xmlChar limit;

    if ((cur == NULL) || (end < 0)) return(-1);
    if ((cur[end] == '\'') || (cur[end] == '"')) limit = cur[end];
    else return(end);
    end++;
    while (cur[end] != 0) {
        if (cur[end] == limit)
	    return(end + 1);
	end++;
    }
    return(-1);
}
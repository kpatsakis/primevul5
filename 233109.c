skipPredicate(const xmlChar *cur, int end) {
    if ((cur == NULL) || (end < 0)) return(-1);
    if (cur[end] != '[') return(end);
    end++;
    while (cur[end] != 0) {
        if ((cur[end] == '\'') || (cur[end] == '"')) {
	    end = skipString(cur, end);
	    if (end <= 0)
	        return(-1);
	    continue;
	} else if (cur[end] == '[') {
	    end = skipPredicate(cur, end);
	    if (end <= 0)
	        return(-1);
	    continue;
	} else if (cur[end] == ']')
	    return(end + 1);
	end++;
    }
    return(-1);
}
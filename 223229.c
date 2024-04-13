void stralgoCommand(client *c) {
    /* Select the algorithm. */
    if (!strcasecmp(c->argv[1]->ptr,"lcs")) {
        stralgoLCS(c);
    } else {
        addReply(c,shared.syntaxerr);
    }
}
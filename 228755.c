enum MIME SFDGetImage2MIME(FILE *sfd) {
    char mime[128];

    if ( !getname(sfd, mime) ) {
        IError("Failed to get a MIME type, file corrupt");
        return UNKNOWN;
    }

    if ( !(strmatch(mime, "image/png")==0) ) {
        IError("MIME type received—%s—is not recognized", mime);
        return UNKNOWN;
    }

    return PNG;
}
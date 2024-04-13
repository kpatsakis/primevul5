static int ask_about_file(char *filename, int *state, FILE **asfd) {
    int ret;
    char *buts[6];
    char buffer[800], *pt;

    if ((*asfd = fopen(filename, "r")) == NULL) {
        return false;
    } else if (*state&1) { //Recover all
        return true;
    } else if (*state&2) { //Forget all
        fclose(*asfd);
        *asfd = NULL;
        unlink(filename);
        return false;
    }

    fgets(buffer,sizeof(buffer),*asfd);
    rewind(*asfd);
    if (strncmp(buffer,"Base: ",6) != 0) {
        strcpy(buffer+6, "<New File>");
    }
    pt = buffer+6;
    if (strlen(buffer+6) > 70) {
        pt = strrchr(buffer+6,'/');
        if (pt == NULL)
            pt = buffer+6;
    }

    buts[0] = _("Yes"); buts[1] = _("Yes to _All");
    buts[2] = _("_Skip for now");
    buts[3] = _("Forget _to All"); buts[4] = _("_Forget about it");
    buts[5] = NULL;
    ret = ff_ask(_("Recover old edit"),(const char **) buts,0,3,_("You appear to have an old editing session on %s.\nWould you like to recover it?"), pt);
    switch (ret) {
        case 1: //Recover all
            *state = 1;
            break;
        case 2: //Skip one
            fclose(*asfd);
            *asfd = NULL;
            return false;
        case 3: //Forget all
            *state = 2;
            /* Fall through */
        case 4: //Forget one
            fclose(*asfd);
            *asfd = NULL;
            unlink(filename);
            return false;
        default: //Recover one
            break;
    }
    return true;
}
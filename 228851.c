static void tterr(void *UNUSED(rubbish), char *message, int UNUSED(pos)) {
    LogError(_("When loading tt instrs from sfd: %s\n"), message );
}
static int my_find_msgid(char *msgid, char **mailbox, uint32_t *uid)
{
    struct findrock frock = { NULL, 0 };

    duplicate_find(msgid, &find_cb, &frock);

    if (!frock.mailbox) return 0;

    if (mailbox) {
	if (!frock.mailbox[0]) return 0;
	*mailbox = (char *) frock.mailbox;
    }
    if (uid) {
	if (!frock.uid) return 0;
	*uid = frock.uid;
    }

    return 1;
}
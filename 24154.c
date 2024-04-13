static int check_message(const struct set_name_fn *fn, const char *op,
                         const char *nameincert, int match, const char *name)
{
    char msg[1024];

    if (match < 0)
        return 1;
    BIO_snprintf(msg, sizeof(msg), "%s: %s: [%s] %s [%s]",
                 fn->name, op, nameincert,
                 match ? "matches" : "does not match", name);
    if (is_exception(msg))
        return 1;
    TEST_error("%s", msg);
    return 0;
}
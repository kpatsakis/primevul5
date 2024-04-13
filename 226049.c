static void session_insert_output_filter(request_rec * r)
{
    ap_add_output_filter("MOD_SESSION_OUT", NULL, r, r->connection);
}
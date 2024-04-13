static void handle_service(HttpRequest req, HttpResponse res) {
        char *name = req->url;
        if (! name) {
                send_error(req, res, SC_NOT_FOUND, "Service name required");
                return;
        }
        Service_T s = Util_getService(++name);
        if (! s) {
                send_error(req, res, SC_NOT_FOUND, "There is no service named \"%s\"", name);
                return;
        }
        do_service(req, res, s);
}
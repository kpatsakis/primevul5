static void doGet(HttpRequest req, HttpResponse res) {
        set_content_type(res, "text/html");
        if (ACTION(HOME)) {
                LOCK(Run.mutex)
                do_home(res);
                END_LOCK;
        } else if (ACTION(RUNTIME)) {
                handle_runtime(req, res);
        } else if (ACTION(TEST)) {
                is_monit_running(res);
        } else if (ACTION(ABOUT)) {
                do_about(res);
        } else if (ACTION(FAVICON)) {
                printFavicon(res);
        } else if (ACTION(PING)) {
                do_ping(res);
        } else if (ACTION(GETID)) {
                do_getid(res);
        } else if (ACTION(STATUS)) {
                print_status(req, res, 1);
        } else if (ACTION(STATUS2)) {
                print_status(req, res, 2);
        } else if (ACTION(SUMMARY)) {
                print_summary(req, res);
        } else if (ACTION(REPORT)) {
                _printReport(req, res);
        } else {
                handle_service(req, res);
        }
}
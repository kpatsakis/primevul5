static void do_home(HttpResponse res) {
        do_head(res, "", "", Run.polltime);
        StringBuffer_append(res->outputbuffer,
                            "<table id='header' width='100%%'>"
                            " <tr>"
                            "  <td colspan=2 valign='top' class='left' width='100%%'>"
                            "  <h1>Monit Service Manager</h1>"
                            "  <p class='center'>Monit is <a href='_runtime'>running</a> on %s and monitoring:</p><br>"
                            "  </td>"
                            " </tr>"
                            "</table>", Run.system->name);

        do_home_system(res);
        do_home_process(res);
        do_home_program(res);
        do_home_filesystem(res);
        do_home_file(res);
        do_home_fifo(res);
        do_home_directory(res);
        do_home_net(res);
        do_home_host(res);

        do_foot(res);
}
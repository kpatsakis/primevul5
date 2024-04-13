static void do_foot(HttpResponse res) {
        StringBuffer_append(res->outputbuffer,
                            "</center></div></div>"
                            "<div id='footer'>"
                            "Copyright &copy; 2001-2018 <a href=\"http://tildeslash.com/\">Tildeslash</a>. All rights reserved. "
                            "<span style='margin-left:5px;'></span>"
                            "<a href=\"http://mmonit.com/monit/\">Monit web site</a> | "
                            "<a href=\"http://mmonit.com/wiki/\">Monit Wiki</a> | "
                            "<a href=\"http://mmonit.com/\">M/Monit</a>"
                            "</div></body></html>");
}
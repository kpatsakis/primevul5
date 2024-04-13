static void do_about(HttpResponse res) {
        StringBuffer_append(res->outputbuffer,
                            "<html><head><title>about monit</title></head><body bgcolor=white>"
                            "<br><h1><center><a href='http://mmonit.com/monit/'>"
                            "monit " VERSION "</a></center></h1>");
        StringBuffer_append(res->outputbuffer,
                            "<ul>"
                            "<li style='padding-bottom:10px;'>Copyright &copy; 2001-2018 <a "
                            "href='http://tildeslash.com/'>Tildeslash Ltd"
                            "</a>. All Rights Reserved.</li></ul>");
        StringBuffer_append(res->outputbuffer, "<hr size='1'>");
        StringBuffer_append(res->outputbuffer,
                            "<p>This program is free software; you can redistribute it and/or "
                            "modify it under the terms of the GNU Affero General Public License version 3</p>"
                            "<p>This program is distributed in the hope that it will be useful, but "
                            "WITHOUT ANY WARRANTY; without even the implied warranty of "
                            "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
                            "<a href='http://www.gnu.org/licenses/agpl.html'>"
                            "GNU AFFERO GENERAL PUBLIC LICENSE</a> for more details.</p>");
        StringBuffer_append(res->outputbuffer,
                            "<center><p style='padding-top:20px;'>[<a href='.'>Back to Monit</a>]</p></body></html>");
}
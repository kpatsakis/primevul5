static void do_head(HttpResponse res, const char *path, const char *name, int refresh) {
        StringBuffer_append(res->outputbuffer,
                            "<!DOCTYPE html>"\
                            "<html>"\
                            "<head>"\
                            "<title>Monit: %s</title> "\
                            "<style type=\"text/css\"> "\
                            " html, body {height: 100%%;margin: 0;} "\
                            " body {background-color: white;font: normal normal normal 16px/20px 'HelveticaNeue', Helvetica, Arial, sans-serif; color:#222;} "\
                            " h1 {padding:30px 0 10px 0; text-align:center;color:#222;font-size:28px;} "\
                            " h2 {padding:20px 0 10px 0; text-align:center;color:#555;font-size:22px;} "\
                            " a:hover {text-decoration: none;} "\
                            " a {text-decoration: underline;color:#222} "\
                            " table {border-collapse:collapse; border:0;} "\
                            " .stripe {background:#EDF5FF} "\
                            " .rule {background:#ddd} "\
                            " .red-text {color:#ff0000;} "\
                            " .green-text {color:#00ff00;} "\
                            " .gray-text {color:#999999;} "\
                            " .blue-text {color:#0000ff;} "\
                            " .yellow-text {color:#ffff00;} "\
                            " .orange-text {color:#ff8800;} "\
                            " .short {overflow: hidden; text-overflow: ellipsis; white-space: nowrap; max-width: 350px;}"\
                            " .column {min-width: 80px;} "\
                            " .left {text-align:left} "\
                            " .right {text-align:right} "\
                            " .center {text-align:center} "\
                            " #wrap {min-height: 100%%;} "\
                            " #main {overflow:auto; padding-bottom:50px;} "\
                            " /*Opera Fix*/body:before {content:\"\";height:100%%;float:left;width:0;margin-top:-32767px;} "\
                            " #footer {position: relative;margin-top: -50px; height: 50px; clear:both; font-size:11px;color:#777;text-align:center;} "\
                            " #footer a {color:#333;} #footer a:hover {text-decoration: none;} "\
                            " #nav {background:#ddd;font:normal normal normal 14px/0px 'HelveticaNeue', Helvetica;} "\
                            " #nav td {padding:5px 10px;} "\
                            " #header {margin-bottom:30px;background:#EFF7FF} "\
                            " #nav, #header {border-bottom:1px solid #ccc;} "\
                            " #header-row {width:95%%;} "\
                            " #header-row th {padding:30px 10px 10px 10px;font-size:120%%;} "\
                            " #header-row td {padding:3px 10px;} "\
                            " #header-row .first {min-width:200px;width:200px;white-space:nowrap;overflow:hidden;text-overflow:ellipsis;} "\
                            " #status-table {width:95%%;} "\
                            " #status-table th {text-align:left;background:#edf5ff;font-weight:normal;} "\
                            " #status-table th, #status-table td, #status-table tr {border:1px solid #ccc;padding:5px;} "\
                            " #buttons {font-size:20px; margin:40px 0 20px 0;} "\
                            " #buttons td {padding-right:50px;} "\
                            " #buttons input {font-size:18px;padding:5px;} "\
                            "</style>"\
                            "<meta HTTP-EQUIV='REFRESH' CONTENT=%d> "\
                            "<meta HTTP-EQUIV='Expires' Content=0> "\
                            "<meta HTTP-EQUIV='Pragma' CONTENT='no-cache'> "\
                            "<meta charset='UTF-8'>" \
                            "<link rel='shortcut icon' href='favicon.ico'>"\
                            "</head>"\
                            "<body><div id='wrap'><div id='main'>" \
                            "<table id='nav' width='100%%'>"\
                            "  <tr>"\
                            "    <td width='20%%'><a href='.'>Home</a>&nbsp;&gt;&nbsp;<a href='%s'>%s</a></td>"\
                            "    <td width='60%%' style='text-align:center;'>Use <a href='https://mmonit.com/'>M/Monit</a> to manage all your Monit instances</td>"\
                            "    <td width='20%%'><p class='right'><a href='_about'>Monit %s</a></td>"\
                            "  </tr>"\
                            "</table>"\
                            "<center>",
                            Run.system->name, refresh, path, name, VERSION);
}
 static bool valid_user_field(const char *p, size_t l) {
         const char *a;
        /* We kinda enforce POSIX syntax recommendations for
           environment variables here, but make a couple of additional
           requirements.

           http://pubs.opengroup.org/onlinepubs/000095399/basedefs/xbd_chap08.html */

        /* No empty field names */
        if (l <= 0)
                return false;

        /* Don't allow names longer than 64 chars */
        if (l > 64)
                return false;

        /* Variables starting with an underscore are protected */
        if (p[0] == '_')
                return false;

        /* Don't allow digits as first character */
        if (p[0] >= '0' && p[0] <= '9')
                return false;

        /* Only allow A-Z0-9 and '_' */
        for (a = p; a < p + l; a++)
                if (!((*a >= 'A' && *a <= 'Z') ||
                      (*a >= '0' && *a <= '9') ||
                      *a == '_'))
                        return false;

        return true;
}

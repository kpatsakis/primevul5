static PHP_GINIT_FUNCTION(libxml)
{
        libxml_globals->stream_context = NULL;
        libxml_globals->error_buffer.c = NULL;
        libxml_globals->error_list = NULL;
 }

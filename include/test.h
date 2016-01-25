typedef struct
{
    const char* current_file;        /* file of current check */
    int current_line;                /* line of current check */
    int todo_level;                  /* current todo nesting level */
    int todo_do_loop;
    char *str_pos;                   /* position in debug buffer */
    char strings[2000];              /* buffer for debug strings */
} tls_data;

static DWORD tls_index;

static tls_data* get_tls_data(void)
{
    tls_data* data;
    DWORD last_error;

    last_error=GetLastError();
    data=TlsGetValue(tls_index);
    if (!data)
    {
        data=HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(tls_data));
        data->str_pos = data->strings;
        TlsSetValue(tls_index,data);
    }
    SetLastError(last_error);
    return data;
}

static char *get_temp_buffer( size_t n )
{
    tls_data *data = get_tls_data();
    char *res = data->str_pos;

    if (res + n >= &data->strings[sizeof(data->strings)]) res = data->strings;
    data->str_pos = res + n;
    return res;
}

/* release extra space that we requested in get_temp_buffer() */
static void release_temp_buffer( char *ptr, size_t size )
{
    tls_data *data = get_tls_data();
    data->str_pos = ptr + size;
}

const char *wine_dbgstr_wn( const WCHAR *str, int n )
{
    char *dst, *res;
    size_t size;

    if (!((ULONG_PTR)str >> 16))
    {
        if (!str) return "(null)";
        res = get_temp_buffer( 6 );
        sprintf( res, "#%04x", LOWORD(str) );
        return res;
    }
    if (n == -1)
    {
        const WCHAR *end = str;
        while (*end) end++;
        n = end - str;
    }
    if (n < 0) n = 0;
    size = 12 + min( 300, n * 5 );
    dst = res = get_temp_buffer( size );
    *dst++ = 'L';
    *dst++ = '"';
    while (n-- > 0 && dst <= res + size - 10)
    {
        WCHAR c = *str++;
        switch (c)
        {
        case '\n': *dst++ = '\\'; *dst++ = 'n'; break;
        case '\r': *dst++ = '\\'; *dst++ = 'r'; break;
        case '\t': *dst++ = '\\'; *dst++ = 't'; break;
        case '"':  *dst++ = '\\'; *dst++ = '"'; break;
        case '\\': *dst++ = '\\'; *dst++ = '\\'; break;
        default:
            if (c >= ' ' && c <= 126)
                *dst++ = c;
            else
            {
                *dst++ = '\\';
                sprintf(dst,"%04x",c);
                dst+=4;
            }
        }
    }
    *dst++ = '"';
    if (n > 0)
    {
        *dst++ = '.';
        *dst++ = '.';
        *dst++ = '.';
    }
    *dst++ = 0;
    release_temp_buffer( res, dst - res );
    return res;
}

static inline const char *wine_dbgstr_w( const WCHAR *s )
{
    return wine_dbgstr_wn( s, -1 );
}

//https://itecnote.com/tecnote/r-define-macro-for-debug-printing-in-c/


#define debug_print(...) \
    do{ if(DEBUG){ \
        fprintf(stderr, __VA_ARGS__); \
    }} while (0)

#define debug_print_detail(...) \
    do{ if(DEBUG){ \
        fprintf(stderr, "%s:%d:%s():", __FILE__, __LINE__, __func__); \
        fprintf(stderr, __VA_ARGS__); \
    }} while (0)

#define debug_pause() \
    do{ if(DEBUG){ \
        getchar(); \
    }} while (0)



typedef struct {

    struct delayed_work work;
    Session *session;

    const char *userBuffer;
    unsigned long userBufferSize;

} DelayedEnqueueOperation;
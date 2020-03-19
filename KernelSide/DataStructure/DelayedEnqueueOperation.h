
typedef struct {

    unsigned long index;

    struct delayed_work work;
    Session *session;

    const char *userBuffer;
    unsigned long userBufferSize;

} DelayedEnqueueOperation;
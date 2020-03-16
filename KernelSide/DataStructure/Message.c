#include "Message.h"

#include <linux/types.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include "../Common/BasicDefines.h"

Message *createMessageFromUserBuffer(const char *userBuffer, size_t userBufferSize) {

    Message *output = kmalloc(sizeof(Message), GFP_KERNEL);
    if (output != NULL) {

        output->content = kmalloc(userBufferSize, GFP_KERNEL);
        if (output->content != NULL) {

            output->size = userBufferSize;
            if (copy_from_user(output->content, userBuffer, userBufferSize) != 0)
            {
                kfree(output->content);
                kfree(output);
            }

        } else {

            kfree(output);
            output = NULL;
        }
    }

    return output;
}

int copyMessageToUserBuffer(Message *input, void *userBuffer, size_t userBufferSize) {

    unsigned long byteNotCopied;

    if (userBufferSize >= input->size)
        byteNotCopied = copy_to_user(userBuffer, input->content, input->size);
    else
        byteNotCopied = copy_to_user(userBuffer, input->content, userBufferSize);

    if (byteNotCopied != 0)
        return FAILURE;
    else
        return SUCCESS;
}

void freeMessage(Message *input) {

    kfree(input->content);
    kfree(input);
}
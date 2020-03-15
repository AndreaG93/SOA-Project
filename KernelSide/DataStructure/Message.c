#include "Message.h"

#include <linux/slab.h>
#include "../BasicOperations/BasicDefines.h"

Message *allocateMessage(void *userBuffer, size_t userBufferSize) {

    Message *output = kmalloc(sizeof(Message), GFP_KERNEL);
    if (output != NULL) {

        output->data = kmalloc(userBufferSize, GFP_KERNEL);
        if (output->data != NULL) {

            output->size = size;
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

unsigned char copyMessageToUserBuffer(Message *input, void *userBuffer, size_t userBufferSize) {

    unsigned long byteNotCopied;

    if (userBufferSize >= input->size)
        byteNotCopied = copy_to_user(input->data, userBuffer, input->size);
    else
        byteNotCopied = copy_to_user(input->data, userBuffer, userBufferSizee);

    if (byteNotCopied != 0)
        return FAILURE;
    else
        return SUCCESS;
}

void freeMessage(Message *input) {

    kfree(input->data);
    kfree(input);
}
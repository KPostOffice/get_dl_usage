#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <dlfcn.h>

typedef struct dl_handle {
    void *handle;
    char filename[256];
    int ref_count;
} dl_handle;

typedef struct dl_handle_ll {
    struct dl_handle *value;
    struct dl_handle_ll *next;
} dl_handle_ll;

struct dl_handle_ll *dl_handle_head = NULL;

static void * (*real_dlsym)(void *, const char *)=NULL;
static void * (*real_dlopen)(const char *filename, int flags)=NULL;
static int (*real_dlclose)(void *handle)=NULL;

extern void *_dl_sym(void *, const char *, void *);
extern void *dlsym(void *handle, const char *name)
{
    FILE *fp;
    struct dl_handle_ll *curr = dl_handle_head;
    pid_t cur_pid = getpid();
    char *filename, *to_put;

    if (real_dlsym == NULL)
        real_dlsym=_dl_sym(RTLD_NEXT, "dlsym", dlsym);

    if (!strcmp(name,"dlsym")) 
        return (void*)dlsym;

    if (handle == RTLD_NEXT || handle == RTLD_DEFAULT)
        return real_dlsym(handle,name);

    size_t needed = snprintf(NULL, 0, "/tmp/dll_sym_%u", cur_pid);  // get length of resulting string to malloc
    filename = malloc(needed);
    sprintf(filename, "/tmp/dll_sym_%u", cur_pid);

    while (curr != NULL)
    {
        // check if handle is present in lookup linked list
        if (curr->value->handle == handle)
        {
            needed = snprintf(NULL, 0, "%s,%s\n", curr->value->filename, name);
            to_put = malloc(needed);
            sprintf(to_put, "%s,%s\n", curr->value->filename, name);

            fp = fopen(filename, "a");
            if (fp == NULL) {
                perror("fopen()");
                return 0;
            }
            fputs(to_put, fp);
            fclose(fp);
            free(to_put);
            break;
        }
        curr = curr->next;
    }
    free(filename);
    return real_dlsym(handle, name);
}


extern void *dlopen(const char *filename, int flags)
{
    void *handle;
    struct dl_handle_ll *temp, *curr = dl_handle_head;
    struct dl_handle *entry;

    if (real_dlopen == NULL)
        real_dlopen = dlsym(RTLD_NEXT, "dlopen");

    handle = real_dlopen(filename, flags);

    if (handle == NULL) 
        return 0;

    while (curr != NULL)
    {
        if (strncmp(curr->value->filename, filename, (uint)255))
        {
            curr->value->ref_count++;
            return handle;
        }
    }

    entry = malloc(sizeof(struct dl_handle));
    strncpy(entry->filename, filename, 255);
    entry->handle = handle;
    entry->ref_count = 1;

    temp = malloc(sizeof(struct dl_handle_ll));

    temp->value = entry;
    temp->next = dl_handle_head;

    dl_handle_head = temp;

    return handle;
}

extern int dlclose(void *handle)
{
    struct dl_handle_ll *prev = NULL, *curr = dl_handle_head;
    if (real_dlclose == NULL)
        real_dlclose = dlsym(RTLD_NEXT, "dlclose");

    while (curr != NULL)
    {
        if (curr->value->handle == handle)
        {
            if (--(curr->value->ref_count) == 0)
            {
                // free from table
                if (prev == NULL) //edge case
                {
                    dl_handle_head = curr->next;
                    free(curr->value);
                    free(curr);
                } else {
                    prev->next = curr->next;
                    free(curr->value);
                    free(curr);
                }
            }
            break;
        }
    }
    return real_dlclose(handle);
}
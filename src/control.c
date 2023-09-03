/*--------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <dirent.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/wait.h>

#include "indi_proxy_internal.h"

/*--------------------------------------------------------------------------------------------------------------------*/

str_t indi_driver_list(STR_t path)
{
    /*----------------------------------------------------------------------------------------------------------------*/

    DIR *dir = opendir(path);

    if(dir == NULL)
    {
        return NULL;
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    indi_dict_t *dict = indi_dict_new();
    indi_list_t *list = indi_list_new();

    indi_dict_put(dict, "<>", indi_string_from("drivers"));

    indi_dict_put(dict, "driver_list", list);

    /*----------------------------------------------------------------------------------------------------------------*/

    struct dirent *entry;

    while((entry = readdir(dir)) != NULL)
    {
        if(strncmp(entry->d_name, "indi_", 5) == 0)
        {
            indi_list_push(list, indi_string_from(entry->d_name + 5));
        }
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    str_t result = indi_dict_to_string(dict);

    /*----------------------------------------------------------------------------------------------------------------*/

    indi_dict_free(dict);

    closedir(dir);

    /*----------------------------------------------------------------------------------------------------------------*/

    return result;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static int indi_add_to_envvar(STR_t name, STR_t value)
{
    /*----------------------------------------------------------------------------------------------------------------*/

    char *cur_value = getenv(name);

    if(cur_value == NULL)
    {
        cur_value = "";
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    char *new_value = (char *) indi_memory_alloc(strlen(value) + 1 + strlen(cur_value) + 1);

    sprintf(new_value, "%s:%s", value, cur_value);

    /*----------------------------------------------------------------------------------------------------------------*/

    int result = setenv(name, new_value, 1);

    indi_memory_free(new_value);

    return result;

    /*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/

static __NORETURN__ int indi_server_exec(indi_list_t *list)
{
    /*----------------------------------------------------------------------------------------------------------------*/

    int idx;

    indi_object_t *obj;

    size_t size = indi_list_size(list);

    str_t *args = indi_memory_alloc((size + 2) * sizeof(STR_t));

    for(indi_list_iter_t iter = INDI_LIST_ITER(list); indi_list_iterate(&iter, &idx, &obj);)
    {
        args[idx + 1] = indi_object_to_cstring(obj);
    }

    args[0x000000] = "indiserver";
    args[size + 1] = /**/NULL/**/;

    /*----------------------------------------------------------------------------------------------------------------*/

    int result = execvp(args[0], args);

    /*----------------------------------------------------------------------------------------------------------------*/

    for(size_t i = 0; i < size; i++)
    {
        indi_memory_free(args[i]);
    }

    indi_memory_free(args);

    indi_list_free(list);

    exit(result);

    /*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/

int indi_server_start(STR_t path, STR_t json)
{
    /*----------------------------------------------------------------------------------------------------------------*/

    if(path == NULL || json == NULL)
    {
        return -1;
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    if(indi_add_to_envvar("DYLD_LIBRARY_PATH", path) < 0
       ||
       indi_add_to_envvar("LD_LIBRARY_PATH", path) < 0
       ||
       indi_add_to_envvar("PATH", path) < 0
    ) {
        return -1;
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    indi_object_t *object = indi_object_parse(json);

    if(object == NULL)
    {
        return -1;
    }

    if(object->type != INDI_TYPE_LIST)
    {
        indi_object_free(object);

        return -1;
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    int pid = fork();

    /**/ if(pid > 0)
    {
        indi_object_free(object);

        return pid;
    }
    else if(pid < 0)
    {
        indi_object_free(object);

        return -1;
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    indi_server_exec((indi_list_t *) object);

    /*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/

int indi_server_stop(int pid)
{
    if(pid > 0)
    {
        int status;

        kill(pid, SIGTERM);

        waitpid(pid, &status, 0);

        if(WIFEXITED(status))
        {
            return WEXITSTATUS(status);
        }
    }

    return -1;
}

/*--------------------------------------------------------------------------------------------------------------------*/

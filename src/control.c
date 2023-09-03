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

static __NORETURN__ int indi_server_exec(STR_t path, indi_list_t *list)
{
    /*----------------------------------------------------------------------------------------------------------------*/

    int idx;

    indi_object_t *obj;

    size_t size = indi_list_size(list);

    str_t *argv = indi_memory_alloc((size + 1) * sizeof(STR_t));

    for(indi_list_iter_t iter = INDI_LIST_ITER(list); indi_list_iterate(&iter, &idx, &obj);)
    {
        argv[idx] = indi_object_to_cstring(obj);
    }

    argv[size] = NULL;

    /*----------------------------------------------------------------------------------------------------------------*/

    char *cur_path = getenv("PATH");

    if(cur_path == NULL)
    {
        cur_path = "";
    }

    size_t path_size = strlen(cur_path) + strlen(path) + 2;

    char *new_path = (char *) indi_memory_alloc(path_size);

    snprintf(new_path, path_size, "%s:%s", path, cur_path);

    setenv("PATH", new_path, 1);

    indi_memory_free(new_path);

    /*----------------------------------------------------------------------------------------------------------------*/

    int result = execv("indiserver", argv);

    /*----------------------------------------------------------------------------------------------------------------*/

    for(size_t i = 0; i < size; i++)
    {
        indi_memory_free(argv[i]);
    }

    indi_memory_free(argv);

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

    indi_server_exec(path, (indi_list_t *) object);

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

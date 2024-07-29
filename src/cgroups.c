#include "cgroups.h"
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void create_cgroup_dir(const char *path)
{
    if (mkdir(path, 0755) != 0 && errno != EEXIST)
    {
        err(EXIT_FAILURE, "mkdir");
    }
}

void write_to_cgroup_file(const char *path, const char *value)
{
    int fd = open(path, O_WRONLY | O_CREAT, 0755);
    printf("status: %d\n", fd);
    printf("path: %s\n", path);
    if (fd == -1)
    {
        err(EXIT_FAILURE, "open");
    }

    if (write(fd, value, strlen(value)) == -1)
    {
        close(fd);
        err(EXIT_FAILURE, "write");
    }

    close(fd);
}

void set_memory_limit(const char *limit)
{
    create_cgroup_dir(MEMORY_CGROUP_PATH);
    write_to_cgroup_file(MEMORY_CGROUP_PATH "/memory.limit_in_bytes", limit);
}

void set_cpu_limit(const char *quota, const char *period)
{
    create_cgroup_dir(CPU_CGROUP_PATH);
    write_to_cgroup_file(CPU_CGROUP_PATH "/cpu.cfs_quota_us", quota);
    write_to_cgroup_file(CPU_CGROUP_PATH "/cpu.cfs_period_us", period);
}

void add_process_to_cgroup(pid_t pid)
{
    char pid_str[20];
    snprintf(pid_str, sizeof(pid_str), "%d", pid);
    write_to_cgroup_file(MEMORY_CGROUP_PATH "/tasks", pid_str);
    write_to_cgroup_file(CPU_CGROUP_PATH "/tasks", pid_str);
}

void check_cgroup_support()
{
    // Implementation for checking if cgroups are supported
}
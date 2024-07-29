#ifndef CGROUPS_H
#define CGROUPS_H

#include <sys/types.h>

#define CGROUP_FOLDER "/sys/fs/cgroup"
#define MEMORY_CGROUP_PATH CGROUP_FOLDER "/memory/container_memory"
#define CPU_CGROUP_PATH CGROUP_FOLDER "/cpu/container_cpu"

void create_cgroup_dir(const char *path);
void write_to_cgroup_file(const char *path, const char *value);
void set_memory_limit(const char *limit);
void set_cpu_limit(const char *quota, const char *period);
void add_process_to_cgroup(pid_t pid);
void check_cgroup_support();

#endif
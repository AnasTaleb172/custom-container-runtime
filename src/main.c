#define _GNU_SOURCE
#include <sys/mman.h>
#include <err.h>
#include <errno.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/utsname.h>
#include <sys/mount.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "cgroups.h"

#define STACK_SIZE (1024 * 1024)                   /* Stack size for cloned child */
#define CONTAINER_PATH "container_filesystem_root" /* Path for the container */

// Apply the mapping for UID & GID
void write_mapping(const char *path, const char *mapping)
{
    FILE *map_file = fopen(path, "w");
    if (map_file == NULL)
    {
        err(EXIT_FAILURE, "fopen %s", path);
    }
    if (fputs(mapping, map_file) == EOF)
    {
        err(EXIT_FAILURE, "fputs %s", path);
    }
    fclose(map_file);
}

int child_func(void *arg)
{
    char **argv = (char **)arg;
    struct utsname uts;

    // Set the name of the child process
    if (prctl(PR_SET_NAME, "container_process") == -1)
    {
        err(EXIT_FAILURE, "prctl");
    }

    // Change root to the container's root filesystem
    if (chroot(CONTAINER_PATH) != 0)
    {
        err(EXIT_FAILURE, "chroot");
    }

    // Change working dir
    if (chdir("/") != 0)
    {
        err(EXIT_FAILURE, "chdir");
    }

    // Mount the /proc filesystem
    if (mount("proc", "/proc", "proc", 0, "") == -1)
    {
        err(EXIT_FAILURE, "mount");
    }

    // Set UID and GID mappings
    write_mapping("/proc/self/setgroups", "deny"); // Deny setting groups for non-root
    write_mapping("/proc/self/uid_map", "0 1000 1\n");
    write_mapping("/proc/self/gid_map", "0 1000 1\n");

    // Set hostname in UTS namespace of child
    if (sethostname("container", 9) == -1)
    {
        err(EXIT_FAILURE, "sethostname");
    }

    // Retrieve and display hostname
    if (uname(&uts) == -1)
    {
        err(EXIT_FAILURE, "uname");
    }

    // Get the command
    if (strcmp(argv[1], "run") == 0)
    {
        // Execute the command
        if (execvp(argv[2], &argv[2]) == -1)
        {
            err(EXIT_FAILURE, "execvp");
        }
    }

    // Unmount the /proc filesystem before exiting
    if (umount("/proc") == -1)
    {
        err(EXIT_FAILURE, "umount");
    }

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    char *stack;
    char *stackTop;
    int status;

    // Check for the arguments
    if (argc < 2)
    {
        errx(EXIT_FAILURE, "no command is specified");
    }

    // Allocate stack for child process
    stack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_STACK, -1, 0);
    if (stack == MAP_FAILED)
    {
        err(EXIT_FAILURE, "mmap");
    }
    stackTop = stack + STACK_SIZE;

    // Set CGROUPS
    // set_memory_limit("100M");
    // set_cpu_limit("50000", "100000");

    // Create child process with new PID and UTS namespaces
    pid_t pid = clone(child_func, stackTop, CLONE_NEWUSER | CLONE_NEWNS | CLONE_NEWUTS | CLONE_NEWPID | SIGCHLD, argv);
    if (pid == -1)
    {
        err(EXIT_FAILURE, "clone");
    }

    // Add the child process (the container) to the created cgroup
    // add_process_to_cgroup(pid);

    // Wait for the child process to terminate
    waitpid(pid, &status, 0);

    // Get the child's exit code and return it
    if (WIFEXITED(status))
    {
        return WEXITSTATUS(status);
    }
    else if (WIFSIGNALED(status))
    {
        return 128 + WTERMSIG(status);
    }
    else
    {
        return EXIT_FAILURE;
    }

    // Free the allocated stack
    munmap(stack, STACK_SIZE);

    return EXIT_SUCCESS;
}
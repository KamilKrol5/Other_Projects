#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>
#include <fcntl.h>

char **split_input(char *line, int *elements_count_return, const char *delimiters);
const char *const whitespace = " \t\n\r";
pid_t groups_pids[10];
int jobs_count = 0;
const int max_jobs_count = 10;
pid_t current_pgid_being_handled = 0;

void initialize_groups_pids() { memset(groups_pids, -1, sizeof(size_t)); }
bool add_group_pid(pid_t pid)
{
    if (jobs_count >= max_jobs_count)
        return false;
    for (int y = 0; y < max_jobs_count; y++)
    {
        if (groups_pids[y] < 0)
        {
            groups_pids[y] = pid;
            break;
        }
    }
    jobs_count++;
    return true;
}
void remove_group_pid(pid_t pid)
{
    for (int y = 0; y < max_jobs_count; y++)
    {
        if (groups_pids[y] == pid)
        {
            groups_pids[y] = -1;
            break;
        }
    }
    jobs_count--;
}

void print_jobs()
{
    printf("Current jobs (%d):\n", jobs_count);
    for (int y = 0; y < max_jobs_count; y++)
    {
        if (groups_pids[y] > 0)
            printf("pgid: %d\n", groups_pids[y]);
    }
}
void start_stopped_job() //if only one job is started, it is taken to the foreground, if more than one - every job is continued
{                        //but none of them is taken to the foreground
    if (jobs_count == 1)
    {
        for (int h = 0; h < max_jobs_count; h++)
        {
            if (groups_pids[h] > 0)
            {
                int status;
                printf("Job %ld moved to the foreground\n", (long)groups_pids[h]);
                //printf("Jobs count: %d\n",jobs_count);
                killpg(groups_pids[h], SIGCONT);
                if (tcsetpgrp(STDOUT_FILENO, groups_pids[h]) < 0)
                    perror("67:tcsetpgrp");
                waitpid(groups_pids[h], &status, WUNTRACED);
                if (!WIFSTOPPED(status))
                    remove_group_pid(groups_pids[h]);
                if (tcsetpgrp(STDOUT_FILENO, getpgrp()) < 0)
                    perror("tcsetpgrp");
            }
        }
        return;
    }
    for (int h = 0; h < max_jobs_count; h++)
    {
        if (groups_pids[h] > 0)
        {
            killpg(groups_pids[h], SIGCONT);
            killpg(groups_pids[h], SIGKILL);
            groups_pids[h] = -1;
        }
    }
    jobs_count = 0;
}

struct redirection
{
    int left;
    int right;
    char type;
};

void SIGINT_handler()
{
}

void SIGCHLD_handler()
{
    pid_t pid;
    //call waitpid with argument -1 to wait for any child process (kernel knows that)
    //WNOHANG prevents handler from blocking, it tells to return 0 if no child has exited
    //in other words, to return pid of child only when it died, not to wait for child if it is executing
    while ((pid = waitpid(-1, 0, WNOHANG)) > 0)
    {
        remove_group_pid(pid);
    } //we could call waitpid only once but we know that signals are not queued, so if more than one child dies
      //at once we want to catch them all
}

void print_start()
{
    char working_directory[PATH_MAX]; //PATH_MAX is max length a path can has
    printf("\x1b[32mlsh\x1b[0m:");
    getcwd(working_directory, PATH_MAX);
    printf("\x1b[93m%s\x1b[0m$", working_directory);
}

bool is_delimiter(char c, const char *delimiters)
{
    for (const char *tmp = delimiters; *tmp != '\0'; tmp++)
    {
        if (c == *tmp)
        {
            return true;
        }
    }
    return false;
}

int run_single_pipe_element(char **pipe_content, int count)
{
    struct redirection reds[count];
    char *arguments[count + 1]; //execvp wait for array ended with NULL
    memset(arguments, 0, (count + 1) * sizeof(*arguments));
    char **split_content;
    int element_count = 0;
    int k = 0;
    int j = 0;
    int right_index = 0;
    bool redirection_found = false;
    for (int i = 0; i < count; i++)
    {
        split_content = split_input(pipe_content[i], &element_count, "<>");
        //for(int f= 0;f<element_count;f++ ) { printf("|%s|\n",split_content[f]); }
        if (element_count > 2 || element_count == 0)
        {
            return -77;
        }
        else
        {
            if (is_delimiter('<', pipe_content[i])) //name of this function has no sense but it simply checks if char c is present in given string
            {
                redirection_found = true;
                struct redirection *red = &reds[k];
                red->type = '<';
                if (is_delimiter(pipe_content[i][0], whitespace) || pipe_content[i][0] == '<')
                {
                    red->left = 0;
                    right_index = 0;
                }
                else
                {
                    if (element_count < 2)
                    {
                        return -134;
                    }
                    right_index = 1;
                    char *status;
                    red->left = strtol(split_content[0], &status, 10);
                    if (*status != '\0')
                    {
                        return -105;
                    }
                }
                if (split_content[right_index][0] == '&')
                {
                    if (split_content[right_index][1] == '\0')
                    {
                        return -109;
                    }
                    else
                    {
                        char *status;
                        reds->right = strtol(split_content[1] + 1, &status, 10);
                        if (*status != '\0')
                        {
                            return -113;
                        }
                    }
                }
                else
                {
                    red->right = open(split_content[right_index], O_RDONLY);
                }
                k++;
            }
            else if (is_delimiter('>', pipe_content[i]))
            {
                redirection_found = true;
                struct redirection *red = &reds[k];
                red->type = '>';
                if (is_delimiter(pipe_content[i][0], whitespace) || pipe_content[i][0] == '>')
                {
                    red->left = 1;
                    right_index = 0;
                }
                else
                {
                    if (element_count < 2)
                    {
                        return -134;
                    }

                    right_index = 1;
                    char *status;
                    red->left = strtol(split_content[0], &status, 10);
                    if (*status != '\0')
                    {
                        return -139;
                    }
                }
                if (split_content[right_index][0] == '&')
                {
                    if (split_content[right_index][1] == '\0')
                    {
                        return -136;
                    }
                    else
                    {
                        char *status;
                        red->right = strtol(split_content[1] + 1, &status, 10);
                        if (*status != '\0')
                        {
                            return -140;
                        }
                    }
                }
                else
                {
                    red->right = open(split_content[right_index], O_WRONLY | O_CREAT, 0777);
                }
                k++;
            }
            else
            {
                if (redirection_found)
                {
                    return -151;
                }
                else
                {
                    arguments[j++] = pipe_content[i]; //can use i since all arguments have to be consecutive
                }
            }
        }
    }
    for (int y = 0; y < k; y++) //assigning(duplicating) file descriptors
    {
        //printf("^%d,%d,%c^\n",reds[y].left,reds[y].right,reds[y].type);
        dup2(reds[y].right, reds[y].left);
        close(reds[y].right);
    }
    // for(char **arg = arguments; *arg != NULL; arg++ )
    // {
    //     printf(";%s;", *arg);
    // }
    //for (int h=0;h < j;h++) {printf("_%s_\n",arguments[h]); }
    //printf("\n");
    execvp(arguments[0], arguments);
    return -246;
}

int read_from_stdin(char **line)
{
    size_t size = 0;                           //size_t is an unsigned integer data type. It's used to represent sizes of objects in bytes
    ssize_t ret = getline(line, &size, stdin); //There could be some file instead of stdin
    if (ret > 0)                               //getline returns number of bytes read and we need to check if line is non-empty
        if ((*line)[ret - 1] == '\n')          //replace \n with \0
            (*line)[ret-- - 1] = '\0';

    return ret;
}

char **split_input(char *line, int *elements_count_return, const char *delimiters)
{
    int buffer_size = 128;
    char **elements = calloc(buffer_size, sizeof(char *));
    char *buffer = malloc(sizeof(char) * buffer_size);
    char *current_buff_ptr = buffer;
    char *current_line_ptr = line;

    int elements_counter = 0;
    bool add_to_elements = true;
    do
    {
        if (is_delimiter(*current_line_ptr, delimiters))
        {
            if (!add_to_elements)
                *current_buff_ptr++ = '\0';
            add_to_elements = true;
            current_line_ptr++;
            continue;
        }
        else
        {
            *current_buff_ptr = *current_line_ptr;
        }
        if (add_to_elements)
        {
            add_to_elements = false;
            elements[elements_counter++] = current_buff_ptr;
        }
        current_line_ptr++;
        current_buff_ptr++;

        if (elements_counter >= buffer_size)
        {
            printf("Full buffer error");
            exit(1);
        }

    } while (*current_line_ptr != '\0');
    *current_buff_ptr = '\0';

    *elements_count_return = elements_counter;
    return elements;
}

int main(int argc, char const *argv[])
{
    initialize_groups_pids();
    //_____________________________________________________SET SIGINT ACTION
    struct sigaction act1;      //create sigaction object, sigaction is a struct from signal.h
    sigemptyset(&act1.sa_mask); //initialize sa_mask in act structure to be empty, the mask tells which signals should
                                //be blocked during executing handler, in this case it's empty
                                //using sigemptyset() because we dont know the type of this mask (sigset_t), it's inner struct on signal.h library
    act1.sa_flags = SA_RESTART; //options which tells how the handler should work
                                //SA_RESTART flag automatically restarts system calls if they were interrupted by a signal
                                //(sending any signal (except ignored ones) interrupts executing system call)
                                //(not all system calls are restarted with this flag)
    act1.sa_handler = SIGINT_handler;
    sigaction(SIGINT, &act1, NULL); //setting sigaction act for signal SIGINT, 3rd arg is place for returnig previous sigaction i.e. act0. (if set)
    //____________________________________________________SET SIGCHLD ACTION
    struct sigaction childaction;
    sigemptyset(&childaction.sa_mask);
    childaction.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    childaction.sa_handler = SIGCHLD_handler;
    sigaction(SIGCHLD, &childaction, NULL);
    //____________________________________________________HANDLE STOPPING SIGNALS
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    //____________________________________________________
    while (1)
    {
        print_start();
        char *out;
        int line_len;
        //______________________________________________HANDLING EXIT
        if ((line_len = read_from_stdin(&out)) == -1 || strcmp(out, "exit") == 0)
            break;
        //______________________________________________HANDLING JOBS CONTROLL
        if (strcmp(out, "jobs") == 0)
        {
            print_jobs();
            continue;
        }
        if (strcmp(out, "fg") == 0)
        {
            start_stopped_job();
            continue;
        }
        //if (strcmp(out, "bg") == 0)  { print_jobs(); continue; }
        //______________________________________________determine if program should be executed in the background
        bool background = false;
        //printf("\"%s\" len=%d", out, line_len);
        if (line_len >= 2)
        {
            if (out[line_len - 1] == '&') //if the last char is '&' we want to run the programm in the background
            {
                background = true;
                out[line_len - 1] = '\0'; //replace '&' with '\0' to avoid giving wrong argument
            }
        }
        int count = 0;
        //_____________________________________________PIPES
        int count_pipe = 0;
        //printf("%s, %d\n",out,line_len);
        char **pipes = split_input(out, &count_pipe, "|");

        //printf("cp=%d\n",count_pipe);
        //if (count_pipe > 1)
        if (count_pipe > 1)
        {
            char **xd = split_input(pipes[0], &count, whitespace);

            int fd[2];
            if (pipe(fd) == -1)
            {
                perror("PIPE ERROR:");
            }
            else
            {
                if (fork() == 0)
                {
                    dup2(fd[1], STDOUT_FILENO);
                    close(fd[1]);
                    int err = 0;
                    if ((err = run_single_pipe_element(xd, count)) < 0)
                    {
                        printf("Running signle pipe element error. %d \n", err);
                        exit(1);
                    }
                    //execvp(xd[0],xd);
                }
                else
                {
                    close(fd[1]);
                    int previous = fd[0];
                    for (int k = 1; k < count_pipe - 1; k++)
                    {
                        xd = split_input(pipes[k], &count, whitespace);
                        pipe(fd);
                        if (fork() == 0)
                        { //child
                            dup2(previous, STDIN_FILENO);
                            close(previous);
                            dup2(fd[1], STDOUT_FILENO);
                            close(fd[1]);
                            int err = 0;
                            if ((err = run_single_pipe_element(xd, count)) < 0)
                            {
                                perror("Error message: ");
                                printf("Wrong input. Error %d \n", err);
                                exit(1);
                            }
                            //execvp(xd[0],xd);
                        }
                        else
                        {
                            close(previous);
                            previous = fd[0];
                            close(fd[1]);
                        }
                    }
                    xd = split_input(pipes[count_pipe - 1], &count, whitespace);
                    pid_t pid;
                    if ((pid = fork()) == 0)
                    {
                        dup2(previous, STDIN_FILENO);
                        close(previous);
                        int err = 0;
                        if ((err = run_single_pipe_element(xd, count)) < 0)
                        {
                            printf("Running signle pipe element error. %d \n", err);
                            exit(1);
                        }
                        //run_single_pipe_element(xd,count);
                        //execvp(xd[0],xd);
                    }
                    else
                    {
                        close(previous);
                        if (!background)
                        {
                            int status;
                            waitpid(pid, &status, 0);
                        }
                        else
                        {
                            printf("Process %d run in the background\n", pid);
                        }
                        continue;
                    }
                }
            }
        }
        else
        {
            char **xd = split_input(out, &count, whitespace);

            // for (int j=0;j<count;j++) {
            //     printf("-%s-\n",xd[j]);
            // }
            if (count != 0)
            {
                //___________________________________________HANDLING CD
                if (strcmp(xd[0], "cd") == 0)
                {
                    //printf("%s;%s\n", out, xd[1]);
                    if (count < 2 || chdir(xd[1]) == -1)
                    {
                        perror(NULL);
                        printf("Wrong direcory\n");
                    }
                }
                else
                {
                    pid_t pid;

                    if ((pid = fork()) == 0)
                    { //child
                        setpgid(getpid(), getpid());
                        if (tcsetpgrp(STDOUT_FILENO, getpgrp()) < 0)
                            perror("tcsetpgrp");
                        signal(SIGTSTP, SIG_DFL);
                        signal(SIGTTIN, SIG_DFL);
                        signal(SIGTTOU, SIG_DFL);
                        int err = 0;
                        if ((err = run_single_pipe_element(xd, count)) < 0)
                        {
                            printf("Wrong input. Error %d \n", err);
                            exit(1);
                        }
                    }
                    else
                    { //parent
                        add_group_pid(pid);
                        if (!background) //if we run process in the background we do not wait, there is a sigchld hanlder to avoid zombie processes
                        {
                            int status;
                            waitpid(pid, &status, WUNTRACED); //wait waits for first child to die, we need to wait for specific child
                            //WUNTRACED makes it to return if a child has stopped (but not traced via ptrace(2)).
                            if (!WIFSTOPPED(status))
                                remove_group_pid(pid);
                        }
                        else
                        {
                            printf("Process %d run in the background\n", pid);
                        }
                        usleep(1000);
                        if (tcsetpgrp(STDOUT_FILENO, getpgrp()) < 0)
                            perror("tcsetpgrp");
                    }
                }
            }
            free(xd[0]);
            free(xd);
            free(pipes[0]);
            free(pipes);
            free(out);
            out = NULL;
        }
    }
    start_stopped_job();
    printf("\n");
    return 0;
}

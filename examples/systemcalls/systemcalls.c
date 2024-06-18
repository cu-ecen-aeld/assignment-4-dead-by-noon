#include "systemcalls.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>

/**
 * @param cmd the command to execute with system()
 * @return true if the command in @param cmd was executed
 *   successfully using the system() call, false if an error occurred,
 *   either in invocation of the system() call, or if a non-zero return
 *   value was returned by the command issued in @param cmd.
*/
bool do_system(const char *cmd)
{

/*
 * TODO  add your code here
 *  Call the system() function with the command set in the cmd
 *   and return a boolean true if the system() call completed with success
 *   or false() if it returned a failure
*/
    // This if statement will NOT execute if and only if the return value of system(cmd) is a 0
    if (system(cmd))
    {
        return false;
    }

    return true; // This indicates that the call to system returns a 0.

}

/**
* @param count -The numbers of variables passed to the function. The variables are command to execute.
*   followed by arguments to pass to the command
*   Since exec() does not perform path expansion, the command to execute needs
*   to be an absolute path.
* @param ... - A list of 1 or more arguments after the @param count argument.
*   The first is always the full path to the command to execute with execv()
*   The remaining arguments are a list of arguments to pass to the command in execv()
* @return true if the command @param ... with arguments @param arguments were executed successfully
*   using the execv() call, false if an error occurred, either in invocation of the
*   fork, waitpid, or execv() command, or if a non-zero return value was returned
*   by the command issued in @param arguments with the specified arguments.
*/

bool do_exec(int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];

    pid_t pid;
    int status;
    fflush(stdout);


    pid = fork ();
    if (pid == -1){
      perror ("fork");
      return false;
    }
    /* the child ... */
    if (!pid) {
      // const char *args[] = { "windlass", NULL };
      int ret;
      ret = execv (command[0], command);
      if (ret == -1) {
        // Returning false if the execv command does not transfer to the called process successfully
        return false;
      }
    }

  // Wait for the child process to terminate and catch the PID of the child using waitpid() call
    if(waitpid(pid, &status, 0) == -1){
      perror ("wait");
      return false;
    }


        va_end(args);

        return true;
}

/**
* @param outputfile - The full path to the file to write with command output.
*   This file will be closed at completion of the function call.
* All other parameters, see do_exec above
*/
bool do_exec_redirect(const char *outputfile, int count, ...)
{
    va_list args;
    va_start(args, count);
    char * command[count+1];
    int i;
    for(i=0; i<count; i++)
    {
        command[i] = va_arg(args, char *);
    }
    command[count] = NULL;
    // this line is to avoid a compile warning before your implementation is complete
    // and may be removed
    command[count] = command[count];

    pid_t pid;
    int status;
    int fd;

    fd = open(outputfile, O_WRONLY|O_TRUNC|O_CREAT, 0644);
    if (fd < 0) { // failed to open file
      return false;
    }


    fflush(stdout);
    pid = fork();
    if (pid == -1) {
        close(fd);
        va_end(args);
        perror ("fork");
        return false;
    }

    if (pid == 0)
    {
        if (dup2(fd, 1) < 0)
        {
            exit(-1); // duplicating file descriptor failed
        }
        close(fd);
        execv(command[0], command);
        exit(-1);
    }

    close(fd);

    // Wait for the child process to terminate and catch the PID of the child using waitpid() call
    if(waitpid(pid, &status, 0) == -1){
      perror ("wait");
    }


    if (waitpid(pid, &status, 0) == -1)
    {
        va_end(args);
        return false;
    }

    if (!WIFEXITED(status))
    {
        va_end(args);
        return false;
    }

    if (WEXITSTATUS(status))
    {
        va_end(args);
        return false;
    }

    va_end(args);
    return true; // successful system call


        va_end(args);

        return true;
}

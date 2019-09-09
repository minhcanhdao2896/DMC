#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    int fd;

    if(2 != argc)
    {
        printf("\n Usage :  \n");
        return 1;
    }

    errno = 0;
    fd = open(argv[1],O_RDONLY);

    if(-1 == fd)
    {
        printf("\n open() failed with error [%s]\n",strerror(errno));
        return 1;
    }
    else
    {
        printf("\n Open() Successful\n");
        /* open() succeeded, now one can do read operations
           on the file opened since we opened it in read-only
           mode. Also once done with processing, the file needs
           to be closed. Closing a file can be achieved using
           close() function. */
    }

    return 0;
}

//LINUX
$ ./open ob.txt

 Open() Successful
$ ./open Music

 open() failed with error [No such file or directory]
$
//LINK
https://www.ibm.com/developerworks/community/blogs/58e72888-6340-46ac-b488-d31aa4058e9c/entry/understanding_linux_open_system_call?lang=en
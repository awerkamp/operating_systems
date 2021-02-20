#include <iostream>
#include <unistd.h>

using namespace std;

int main(int argc, char **argv) {

    int msg_pipe[2];
    int msg_size_pipe[2];
    pipe(msg_pipe);
    pipe(msg_size_pipe);

    pid_t pid = fork();

    if (pid > 0) {
        // parent process

        close (msg_size_pipe[0]);
        close (msg_pipe[0]);

        string message;

        for (int i = 1; i < argc; i++) {
            message += argv[i];
            if (i < argc - 1) {
                message += " ";
            }
        }

        int size = message.size();

        char newMessage[size];

        for (int i = 0; i < size; i++) {
            newMessage[i] = message[i];
        }

        write (msg_pipe[1], newMessage, size + 1);
        close (msg_pipe[1]);

        char newSize[1];
        newSize[0] = (char)size;

        write(msg_size_pipe[1], newSize, 2);
        close (msg_size_pipe[1]);

        cout << "Parent Sent Message: Waiting for Child" << endl;
        wait(nullptr);

        cout << "Parent Exiting" << endl;
        return EXIT_SUCCESS;
    }

    else if (pid == 0) {
        // child process

        close(msg_size_pipe[1]);
        close(msg_pipe[1]);

        char size[1];

        read(msg_size_pipe[0], size, 2);
        close(msg_size_pipe[0]);

        int found_size = (unsigned char)size[0];

        char message[found_size];

        read(msg_pipe[0], message, found_size + 1);
        close(msg_pipe[0]);

        string final_message;
        for (int i = 0; i < found_size; i++) {
            final_message += message[i];
        }
        cout << "Child Received Message: " << final_message << endl;

        cout << "Child Exiting" << endl;
        return EXIT_SUCCESS;
    }

    else {
        // fork failed
        printf("fork() failed!\n");
        return EXIT_FAILURE;
    }
    return 0;
}

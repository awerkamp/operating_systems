#include <iostream>
#include <unistd.h>
#include <cerrno>

using namespace std;

void check_sys_call_code(int sys_call_code) {
    if (sys_call_code == -1) {
        perror("Error: ");
        std::cout << strerror(errno) << std::endl;
    }
}

int main(int argc, char **argv) {

    int msg_pipe[2];
    int msg_size_pipe[2];
    int sys_call_code;
    sys_call_code = pipe(msg_pipe);
    check_sys_call_code(sys_call_code);
    sys_call_code = pipe(msg_size_pipe);
    check_sys_call_code(sys_call_code);

    pid_t pid = fork();

    if (pid > 0) {
        // parent process
        cout << "Parent" << endl;

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

        sys_call_code = write (msg_pipe[1], newMessage, size + 1);
        check_sys_call_code(sys_call_code);
        close (msg_pipe[1]);

        char newSize[1];
        newSize[0] = (char)size;

        sys_call_code = write(msg_size_pipe[1], newSize, 2);
        check_sys_call_code(sys_call_code);
        close (msg_size_pipe[1]);

        cout << "Parent Sent Message" << endl << "Waiting for Child" << endl;
        wait(nullptr);

        cout << "Parent Exiting" << endl;
        return EXIT_SUCCESS;
    }

    else if (pid == 0) {
        // child process
        cout << "Child" << endl;

        close(msg_size_pipe[1]);
        close(msg_pipe[1]);

        char size[1];

        sys_call_code = read(msg_size_pipe[0], size, 2);
        check_sys_call_code(sys_call_code);
        close(msg_size_pipe[0]);

        int found_size = (unsigned char)size[0];

        char message[found_size];

        sys_call_code = read(msg_pipe[0], message, found_size + 1);
        check_sys_call_code(sys_call_code);
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

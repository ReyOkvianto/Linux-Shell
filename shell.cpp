#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

//eliminate spaces before and after a string
string trim_spaces (string input){
    // cout << "Entered the trim function!" << endl;
    // cout << "Original string size: " << input.size() << endl;
    string new_input;
    int counter = 0;
    int before_spaces = 0;
    int after_spaces = 0;
    char placeholder = input[0]; //start at the beginning of string
    // cout << "placeholder: " << placeholder << endl;
    if(input[0] == ' ') { //find number of preceding spaces
        while(placeholder == ' ') {
            // cout << "Space found" << endl;
            counter++;
            before_spaces++;
            placeholder = input[counter];
            // cout << "Counter: " << counter << endl;
        }
        new_input = input.substr(counter, input.size() - counter);
    } else {
        new_input = input;
    }
    //find spaces afterwards
    counter = new_input.size() - 1;
    placeholder = new_input[counter];
    while (placeholder == ' ') {
        // cout << "after space found" << endl;
        after_spaces++;   
        counter--;
        // cout << counter << endl;
        placeholder = new_input[counter];
    }
    // cout << "Before spaces: " << before_spaces << endl;
    // cout << "After spaces: " << after_spaces << endl;
    new_input = new_input.substr(0, new_input.size() - after_spaces);
    // cout << "New string size: " << new_input.size() << endl;
    return new_input;
}

//given a vector, convert each element to a char pointer
char** vector_to_char_array (vector <string>& parts){
    char** result = new char* [parts.size() + 1]; //add 1 for NULL bit
    for (int i=0; i < parts.size(); i++){
        result[i] = (char*) parts.at(i).c_str();
    }
    result[parts.size()] = NULL;
    return result;
}

//split the user input into seperate arguments
//ex: ls -l -w -a
vector <string> split (string line, string separator = " "){
    vector <string> parsed_string;
    string parsed_element;
    char placeholder = line[0];
    int counter = 0;
    // cout << "Line size: " << line.size() << endl;
    while(counter < line.size()) {
        if(placeholder != ' ') {
            parsed_element.push_back(placeholder);
        } else if (placeholder == ' ') {
            parsed_string.push_back(parsed_element);
            parsed_element.clear();
        }
        counter++;
        // cout << "Counter: " << counter << "  ";
        placeholder = line[counter];
        // cout << "Placeholder: " << placeholder << endl;
    }
    parsed_string.push_back(parsed_element);
    return parsed_string;
}

int main() {
    // int kb = dup(0);
    //int terminal = dup(1);
    //why reap? -> save computer resources
    vector <int> bgs; //list of bgs?
    while(true) {
        //check background processes
        for(int i=0; i < bgs.size(); i++){
            //reap zombie processes
            if(waitpid(bgs[i], 0, WNOHANG) < 0){
                cout << "Process: " << bgs[i] << " ended." << endl;
                bgs.erase(bgs.begin() + i);
                i--;
            }
        }

        cout << "My Shell$ ";
        string LineInput;

        // dup2(kb, 0);
        //dup2(terminal, 1);

        getline(cin, LineInput); //get a line from the strandard input
        if (LineInput == string("exit")){
            cout << "End of Shell..." << endl;
            break;
        }
        // cout << "LineInput: " << LineInput << endl;
        bool background = false;
        LineInput = trim_spaces(LineInput);
        // cout << "Testing for trim function: " << LineInput << endl;

        // cout << "Testing the split function..." << endl;
        // vector <string> print = split(LineInput);
        // for(auto element:print){
        //     cout << element << endl;
        //}

        //determine if there is a bg process or not
        if(LineInput[LineInput.size() - 1] == '&') {
            //cout << "Background process found!" << endl;
            background = true;
            LineInput = LineInput.substr(0, LineInput.size() - 1); //remove & 
        }

        //Piping
        vector <string> piped_parts = split(LineInput, "|");
        for(int i = 0; i < piped_parts.size(); i++){
            int fds[2];
            pipe(fds);

            int pid = fork();
            if(pid == 0){ //child process

                if(trim_spaces(LineInput).find("pwd") == 0) {
                    char cwd[1024];
                    getcwd(cwd, sizeof(cwd));
                    printf("%s", cwd);
                }

                if(trim_spaces(LineInput).find("cd") == 0) {
                    string DIR_name = trim_spaces(split(LineInput, " ")[1]);
                    chdir(DIR_name.c_str());
                    continue;
                }

                //Input redirection
                int alligator_index = LineInput.find('>');
                if(alligator_index >= 0){
                    LineInput = trim_spaces(LineInput);
                    string commands = LineInput.substr(0, alligator_index - 1); //ls..
                    string filename = LineInput.substr(alligator_index + 2); //a.txt

                    LineInput = commands;
                    int fd = open(filename.c_str(), O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
                    dup2(fd,1);
                    close(fd);

                }

                //Output Redirection
                alligator_index = LineInput.find('<');
                if(alligator_index >= 0){
                    LineInput = trim_spaces(LineInput);
                    string commands = LineInput.substr(0, alligator_index - 1); //ls..
                    string filename = LineInput.substr(alligator_index + 2); //a.txt

                    LineInput = commands;
                    int fd = open(filename.c_str(), O_RDONLY|O_CREAT, S_IRUSR|S_IWUSR);
                    dup2(fd,0);
                    close(fd);

                }

                if(i < piped_parts.size() - 1){
                    dup2(fds[1], 1);
                }

                vector <string> parts = split(LineInput);
                char** args = vector_to_char_array(parts);
                execvp(args[0], args);
            } else {
                if(!background)
                    waitpid (pid, 0, 0); //wait for the child process
                else{
                    bgs.push_back(pid); //keep track of background processes
                }
                // waitpid(pid, 0, 0);
                // dup2(fds[0], 0);
                // close(fds[1]);
            }

        }
    
        // int pid = fork();
        // if(pid == 0){ //child process
        //     //Input redirection
        //     int alligator_index = LineInput.find('>');
        //     if(alligator_index >= 0){
        //         LineInput = trim_spaces(LineInput);
        //         string commands = LineInput.substr(0, alligator_index - 1); //ls..
        //         string filename = LineInput.substr(alligator_index + 2); //a.txt

        //         LineInput = commands;
        //         int fd = open(filename.c_str(), O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
        //         dup2(fd,1);
        //         close(fd);

        //     }

        //     //Output Redirection
        //     alligator_index = LineInput.find('<');
        //     if(alligator_index >= 0){
        //         LineInput = trim_spaces(LineInput);
        //         string commands = LineInput.substr(0, alligator_index - 1); //ls..
        //         string filename = LineInput.substr(alligator_index + 2); //a.txt

        //         LineInput = commands;
        //         int fd = open(filename.c_str(), O_RDONLY|O_CREAT, S_IRUSR|S_IWUSR);
        //         dup2(fd,0);
        //         close(fd);

        //     }

        //     vector <string> parts = split(LineInput);
        //     char** args = vector_to_char_array(parts);
        //     execvp(args[0], args);
        // } else {
        //     if(!background)
        //         waitpid (pid, 0, 0); //wait for the child process
        //     else{
        //         bgs.push_back(pid); //keep track of background processes
        //     }
        // }   
    }

}
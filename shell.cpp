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
#include <ctime>

using namespace std;

//eliminate spaces before and after a string
string trim_spaces (string input){
    string new_input;
    int counter = 0;
    int before_spaces = 0;
    int after_spaces = 0;
    char placeholder = input[0]; //start at the beginning of string
    if(input[0] == ' ') { //find number of preceding spaces
        while(placeholder == ' ') {
            counter++;
            before_spaces++;
            placeholder = input[counter];
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
    new_input = new_input.substr(0, new_input.size() - after_spaces);
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
        if(placeholder != separator[0]) {
            parsed_element.push_back(placeholder);
        } else if (placeholder == separator[0]) {
            parsed_string.push_back(parsed_element);
            parsed_element.clear();
        }
        counter++;
        // cout << "Counter: " << counter << "  ";
        placeholder = line[counter];
        // cout << "Placeholder: " << placeholder << endl;
    }
    parsed_string.push_back(trim_spaces(parsed_element));
    return parsed_string;
}

int main() {

    dup2(0, 3);

    vector <int> bgs; //list of bgs?
    while(true) {
        //check background processes
        for(int i=0; i < bgs.size(); i++){
            if(waitpid(bgs[i], 0, WNOHANG) == bgs[i]); {
            //reap zombie processes
                cout << "Zombie Process: " << bgs[i] << " reaped." << endl;
                bgs.erase(bgs.begin() + i);
                i--;
            }    
        }

        time_t current_time;
        struct tm* TI;
        time (&current_time);
        TI = localtime(&current_time);
        // char* date_time = ctime(&current_time);


        string Time_t = asctime(TI);
        Time_t = Time_t.substr(0, Time_t.size() - 1);

        string user = getenv("USER");
        cout << user << "(" << Time_t << "):$ ";
        string LineInput;
        dup2(3, 0);

        getline(cin, LineInput); //get a line from the strandard input
        if (LineInput == string("exit")){
            cout << "End of Shell..." << endl;
            break;
        }  

        bool background = false;
        LineInput = trim_spaces(LineInput);
        if(LineInput[LineInput.size() - 1] == '&'){
            background = true;
            LineInput = LineInput.substr(0, LineInput.size() - 1);
        }

        if(trim_spaces(LineInput).find("echo") == 0) {
            string echo;
            int single_index = LineInput.find("'");
            int double_index = LineInput.find("\"");
            if(single_index >= 0) {
                echo = trim_spaces(LineInput.substr(single_index + 1, LineInput.size() - 2 - single_index));
                cout << echo << endl;
            } else if(double_index >= 0){
                echo = trim_spaces(LineInput.substr(double_index + 1, LineInput.size() - 2 - double_index));
                cout << echo << endl;
            }
            continue;
        }

        //Piping
        vector <string> piped_parts = split(LineInput, "|");
        // for(auto i = 0; i < piped_parts.size(); i++){
        //     piped_parts[i] = trim_spaces(piped_parts[i]);
        //     cout << piped_parts[i] << endl;
        // }

        for(int i = 0; i < piped_parts.size(); i++){
            
            bool awk_command = false;
            LineInput = trim_spaces(piped_parts[i]);

            int fds[2];
            pipe(fds);

            int pid = fork();

            if(pid == 0){ //child process

                vector <string> parts;

                // if(trim_spaces(LineInput).find("awk") == 0){
                //     int quote_index = LineInput.find('\'');
                //     LineInput = LineInput.substr(quote_index);
                //     awk_command = true;
                //     parts.push_back("awk");
                //     // cout << "we found an awk lolz" << endl;
                //     for(auto i = 0; i < LineInput.size(); i++){
                //         if(LineInput[i] == '\''){
                //             LineInput.erase(i, 1);
                //         }
                //     }
                //     //cout << "LineInput pushed into vector: " << LineInput << endl;
                //     parts.push_back(LineInput);

                // } 
                // cout << parts[i] << parts[i].size() <<endl;
                // cout << parts[i + 1] << parts[i+1].size() << endl;

                if(trim_spaces(LineInput).find("cd") == 0) {
                    string DIR_name = trim_spaces(split(LineInput, " ")[1]);
                    chdir(DIR_name.c_str());
                    continue;
                }

                if(trim_spaces(LineInput).find("pwd") == 0) {
                    char cwd[1000];
                    getcwd(cwd, sizeof(cwd));
                    printf("%s", cwd);
                    cout << endl;
                    continue;
                }

                //Input redirection
                int alligator_index = LineInput.find('>');
                if(alligator_index >= 0){
                    LineInput = trim_spaces(LineInput);
                    string commands = trim_spaces(LineInput.substr(0, alligator_index)); //ls..
                    string filename = trim_spaces(LineInput.substr(alligator_index + 1)); //a.txt
                    
                    LineInput = trim_spaces(commands);
                    int fd = open(filename.c_str(), O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
                    dup2(fd,1);
                    close(fd);

                }

                //Output Redirection
                alligator_index = LineInput.find('<');
                // cout << "Output index: " << alligator_index << endl;
                if(alligator_index >= 0){
                    LineInput = trim_spaces(LineInput);
                    string commands = trim_spaces(LineInput.substr(0, alligator_index)); //ls..
                    string filename = trim_spaces(LineInput.substr(alligator_index + 1)); //a.txt
                
                    LineInput = commands;
                    //cout << "LineInput after parsing in <: " << LineInput << endl;
                    //cout << "LineInput size: " << LineInput.size() << endl;
                    //cout << "Filename: " << filename << endl;
                    //cout << "Filename size: " << filename.size() << endl;
                    int fd = open(filename.c_str(), O_RDONLY|O_CREAT, S_IRUSR|S_IWUSR);
                    dup2(fd,0);
                    close(fd);
                }
                //cout << "LineInput after output RD: " << LineInput << endl;

                if(trim_spaces(LineInput).find("awk") == 0){
                    int quote_index = LineInput.find('\'');
                    LineInput = LineInput.substr(quote_index);
                    awk_command = true;
                    parts.push_back("awk");
                    // cout << "we found an awk lolz" << endl;
                    for(auto i = 0; i < LineInput.size(); i++){
                        if(LineInput[i] == '\''){
                            LineInput.erase(i, 1);
                        }
                    }
                    //cout << "LineInput pushed into vector: " << LineInput << endl;
                    parts.push_back(LineInput);

                } 

                if(i < piped_parts.size() - 1){
                    dup2(fds[1], 1);
                    close(fds[1]);
                }
                if(awk_command == false) {
                    parts = split(LineInput);
                }
                //vector <string> parts = split(LineInput);
                //cout << "LineInput after before execvp: " << LineInput << endl;
                char** args = vector_to_char_array(parts);
                execvp(args[0], args);
            } else {
                if(!background) {
                    if(i == piped_parts.size() - 1){
                        waitpid (pid, 0, 0); //wait for the child process
                    } else {
                        bgs.push_back(pid);
                    }
                }else{
                    bgs.push_back(pid); //keep track of background processes
                }
                dup2(fds[0],0);
                close(fds[1]);
            }
       
        }

    }
}
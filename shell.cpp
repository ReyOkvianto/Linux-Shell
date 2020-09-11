#include <iostream>
//more libraries
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <stdlib.h>
#include <string.h>

using namespace std;

//eliminate spaces before and after a string
string trim_spaces (string input){}

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
vector <string> split (string line, string separator=" "){}

int main() {

//use split function to separate the input from user
//store into a vector
//convert the string into char pointers
//char** args = vec_to_char_array (parts)
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
        getline(cin, LineInput); //get a line from the strandard input
        if (LineInput == string("exit")){
            cout << "End of Shell..." << endl;
            break;
        }

        bool background = false;
        LineInput = trim_spaces(LineInput);
        //determine if there is a bg process or not
        if(LineInput[LineInput.size() - 1] == '&') {
            //cout << "Background process found!" << endl;
            background = true;
            LineInput = LineInput.substr(0, LineInput.size() - 1); //remove & 
        }
    
        int pid = fork();
        if(pid == 0){ //child process
            vector <string> parts = split(LineInput);
            char** args = vector_to_char_array(parts);
            execvp(args[0], args);
        } else {
            if(!background)
                waitpid (pid, 0, 0); //wait for the child process
            else{
                bgs.push_back(pid); //keep track of background processes
            }
        }   
    }

}
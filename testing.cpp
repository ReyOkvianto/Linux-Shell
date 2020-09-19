#include <iostream>
#include <vector>
#include <string.h>

using namespace std;


//eliminate spaces before and after a string
string trim_spaces (string input){
    cout << "Entered the trim function!" << endl;
    cout << "Original string size: " << input.size() << endl;
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
    cout << "Before spaces: " << before_spaces << endl;
    cout << "After spaces: " << after_spaces << endl;
    new_input = new_input.substr(0, new_input.size() - after_spaces);
    cout << "New string size: " << new_input.size() << endl;
    return new_input;
}

//split the user input into seperate arguments
//ex: ls -l -w -a
vector <string> split (string line){
    vector <string> parsed_string;
    string parsed_element;
    char placeholder = line[0];
    int counter = 0;
    cout << "Line size: " << line.size() << endl;
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

    string string1 = "peanut";
    string string2 = "     peanut";
    string string3 = "    peanut      ";
    string string4 = "  peanut            peanut       ";
    string parsed1 = "ls -a -l";
    string parsed2 = "   ls -a -l     ";
    string parsed3 = "        ls -l | -a -w       ";
    string updatedString = trim_spaces(parsed3);
    // cout << trim_spaces(parsed2) << endl << endl << endl;
    
    vector <string> print = split(updatedString);
    for(auto element:print){
        cout << element << endl;
    }

    return 0;
}
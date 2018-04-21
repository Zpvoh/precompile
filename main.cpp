#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
using namespace std;

map<string,string> definations;

struct instruct{
    string type;
    vector<string> params;
};

vector<int> if_stack;

void handle_define(instruct ins);
void handle_include(string filename, string outputname);

void read_file(string filename, string outputname){
    string line, word;
    ifstream file(filename.c_str());
    ofstream output(outputname.c_str());

    while(getline(file, line)){
        //cout<<line<<endl;
        if(if_stack.empty() || if_stack.front()){
            if(line.at(0)=='#'){
                istringstream iss(line);
                instruct ins;
                iss>>ins.type;
                //cout<<ins.type<<endl;
                while(iss>>word){
                    ins.params.push_back(word);
                    //cout<<ins.params.front()<<endl;
                }
                
                if(ins.type=="#define"){
                    handle_define(ins);
                }else if(ins.type=="#if"){
                    int num;
                    istringstream iss(ins.params[0]);
                    iss>>num;
                    if_stack.push_back(num);
                }else if(ins.type=="#else"){
                    int num=if_stack.front();
                    if_stack.pop_back();
                    num=!num;
                    if_stack.push_back(num);
                }else if(ins.type=="#ifdef"){
                    int num=definations.count(ins.params[0]);
                    if_stack.push_back(num);
                }else if(ins.type=="#ifndef"){
                    int num=definations.count(ins.params[0]);
                    if_stack.push_back(!num);
                }else if(ins.type=="#endif"){
                    if_stack.pop_back();
                }else if(ins.type=="#undef"){
                    definations.erase(ins.params[0]);
                }else if(ins.type=="#include"){
                    read_file(ins.params[0], outputname);
                }else{
                    cout<<"Not Support"<<endl;
                }
            }else{
                cout<<line<<endl;
            }

        }
    }

    file.close();
    output.close();
}

int main() {
    //cout << "Hello, World!" << endl;

    read_file("demo.cpp", "result.txt");

    return 0;
}

void handle_define(instruct ins){
    if(ins.params.size()==1){
        definations.insert(pair<string, string>(ins.params[0], ins.params[0]));
        cout<<ins.params[0]<<" "<<definations[ins.params[0]]<<endl;
    }else if(ins.params.size()==2){
        definations.insert(pair<string, string>(ins.params[0], ins.params[1]));
        cout<<ins.params[0]<<" "<<definations[ins.params[0]]<<endl;
    }
}



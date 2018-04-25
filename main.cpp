#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <regex>

using namespace std;

map<string,string> definations;
map<string,string>::iterator it;

ofstream output;

struct instruct{
    string type;
    vector<string> params;
};

vector<int> if_stack;

void handle_define(instruct ins);
void handle_include(string filename, string outputname);
string replace_line(string line);

void read_file(string filename);

int main(int argc, char** argv) {
    //cout << "Hello, World!" << endl;

    output.open(argv[2]);
    read_file(argv[1]);
    output.close();

    return 0;
}

void handle_define(instruct ins){
    if(ins.params.size()==1){
        definations.insert(pair<string, string>(ins.params[0], ins.params[0]));
        //cout<<ins.params[0]<<" "<<definations[ins.params[0]]<<endl;
    }else if(ins.params.size()==2){

        definations.insert(pair<string, string>(ins.params[0], ins.params[1]));
        //cout<<ins.params[0]<<" "<<definations[ins.params[0]]<<endl;
    }
}

string replace_line(string line){

        for(int i=0; i<definations.size(); i++){
            for(it=definations.begin(); it!=definations.end(); it++){
                if(it->first.find("(")!=string::npos){
                    size_t start=it->first.find("(");
                    size_t end=it->first.find(")");
                    string key=it->first.substr(0, start);
                    string arg=it->first.substr(start+1, end-start-1);
                    regex e(key+"\\(([^ ]*)\\)");
                    string line_fix=line;

                    smatch m;
                    while(regex_search(line_fix, m, e)){
                        regex ex(arg);
                        string value_fixed=regex_replace(it->second, ex, m.format("$1"));
                        string key_fixed=regex_replace(it->first, ex, "\\("+m.format("$1")+"\\)");


                        if(value_fixed.find("#")!=string::npos){
                            ex.assign("#([^]*)");
                            value_fixed=regex_replace(value_fixed, ex, "\"$1\"");
                        }

                        ex.assign(key_fixed);

                        line=regex_replace(line, ex, value_fixed);
                        line_fix=m.suffix().str();
                    }

                }else{

                regex e(it->first);
                string fmt=it->second;
                line=regex_replace(line, e, fmt);
                //cout<<line<<endl;
                }
            //cout<<line<<endl;
            }
        }

        return line;
}

void read_file(string filename){
    string line, word;
    ifstream file(filename.c_str());
    while(getline(file, line)){
        if((if_stack.empty() || if_stack.back()) && line.find("#endif")==string::npos && line.find("#else")==string::npos){
            if(!line.empty() && line.at(0)=='#'){
                istringstream iss(line);
                instruct ins;
                iss>>ins.type;
                //cout<<ins.type<<endl;
                iss>>word;
                ins.params.push_back(word);
                
                string tmp; 
                iss>>word;
                while(iss>>tmp){
                    word=word+" "+tmp;
                    //cout<<(!tmp.find("\"")==string::npos)<<endl;
                    //cout<<ins.params.front()<<endl;
                }
                ins.params.push_back(word);

                if(ins.type=="#define"){
                    handle_define(ins);
                }else if(ins.type=="#if"){
                    int num;
                    for(int i=0; i<definations.size(); i++){
                        if(definations.count(ins.params[0]))
                            ins.params[0]=definations[ins.params[0]];
                    }
                    istringstream iss(ins.params[0]);
                    iss>>num;
                    //cout<<num<<endl;
                    if_stack.push_back(num);
                }else if(ins.type=="#ifdef"){
                    int num=definations.count(ins.params[0]);
                    if_stack.push_back(num);
                }else if(ins.type=="#ifndef"){
                    int num=definations.count(ins.params[0]);
                    if_stack.push_back(!num);
                }else if(ins.type=="#undef"){
                    definations.erase(ins.params[0]);
                }else if(ins.type=="#include" && ins.params[0].find("\"")!=string::npos && ins.params[0].find(".h")!=string::npos){

                    size_t start_pos=ins.params[0].find("\"")+1;
                    size_t end_pos=ins.params[0].rfind("\"")-1;
                    string new_filename =ins.params[0].substr(start_pos, end_pos-start_pos+1);
                    read_file(new_filename);
                }else{
                    line=replace_line(line);
                    output<<line<<endl;
                }
            }else{
                line=replace_line(line);
                output<<line<<endl;
            }

        }else if(line.find("#endif")!=string::npos){
            if_stack.pop_back();
        }else if(line.find("#else")!=string::npos){
            if_stack.back()=!if_stack.back();
        }
    }

    file.close();
}


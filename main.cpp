#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>
#include <functional>
#include <sys/param.h>
#include <unistd.h>
#include <filesystem>
#include <fstream>

#include <unistd.h>

using namespace std;

unordered_map<string, vector<string>> files_dir;

vector<string> command_history;

void ls(string cur_dir)
{

    cur_dir = filesystem::current_path();

    vector<string> files;

    for (auto const &dir_entry : filesystem::directory_iterator{cur_dir})
    {
        files.push_back(dir_entry.path().filename());
    }
    files_dir[cur_dir] = files;
    for (string s : files)
    {
        cout << s << "\n";
    }
}
void cd(string new_dir)
{
    string old_dir = filesystem::current_path();
    filesystem::current_path(new_dir);
}

void pwd(string dummy)
{
    string cwd = filesystem::current_path();
    cout << cwd << "\n";
}

void mkdir(string path)
{
    // filesystem::current_path(filesystem::temp_directory_path());
    filesystem::create_directory(path);
    cout << "\ncreated Directory";
}

void touch(string filename)
{
    string cur_dir = filesystem::current_path();
    ofstream file;

    string fullpath = cur_dir + "/" + filename;
    // cout<<fullpath;
    file.open(fullpath);
    cout << "created file\n";
}

void cp(string source)
{
    string destination;

    cout << "\nEnter destination\n";
    cin >> destination;

    ifstream in_file(source);
    ofstream op_file(destination);

    if (in_file && op_file)
    {
        string line;

        while (getline(in_file, line))
        {
            op_file << line << "\n";
        }
        cout << "copy finished\n";
    }
    else
    {
        cout << "One of the files not found\n";
    }
}

void history(string dummy)
{
    cout << "\nPrinting command history\n";
    for (string s : command_history)
    {
        cout << s << "\n";
    }
}

void clearhistory(string dummy)
{
    cout << "\nClearing commmand history\n";
    command_history.clear();
}

int main()
{
    unordered_map<string, function<void(const string &)>> function_map;

    // All the commands in the map are the ones i explicitly coded
    function_map["ls"] = ls;
    function_map["cd"] = cd;
    function_map["pwd"] = pwd;
    function_map["mkdir"] = mkdir;
    function_map["touch"] = touch;
    function_map["cp"] = cp;
    function_map["history"] = history;
    function_map["clearhistory"] = clearhistory;

    while (1)
    {
        string userinputcommand;
        cout << "\nenter command\n";
        cin >> userinputcommand;

        string userinputdata;

        // userinputdata is usually what you pass along with the command like cd PATH or touch FILENAME

        

        if (function_map.find(userinputcommand) != function_map.end())
        {
            if (userinputcommand != "pwd" && userinputcommand != "ls" && userinputcommand != "history")
        {
            cout << "\nenter data(file/directory/other)\n";
            cin >> userinputdata;
        }
        else
            userinputdata = "";
            function_map[userinputcommand](userinputdata);

            command_history.push_back(userinputcommand);
        }

        else
        {
            pid_t pid = fork();
            if (pid == 0)
            {
                string exec_command = "/bin/" + userinputcommand;

                char *args[3];
                string input;
                //some commands dont require a second argument hence the option
                
                int choice=0;
                cout<<"do you want an argument: 1 for yes 0 for no\n";
                cin>>choice;

                if(choice==1){
                    cout<<"Enter input\n";
                    cin>>input;
                }

                args[0] = const_cast<char *>(exec_command.c_str());
                if (!input.empty())
                {
                    args[1] = const_cast<char *>(input.c_str());
                    args[2] = nullptr;
                }
                else
                {
                    args[1] = nullptr;
                }
                execv(args[0], args);
            }
            else if (pid < 0)
            {

                cerr << "Fork failed\n";
            }
            else
            {

                wait(nullptr);
            }
        }
    }

    return 0;
}
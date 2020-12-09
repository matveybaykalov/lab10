#include <iostream>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;
using std::cout;
using std::cin;
using std::endl;
using std::string;

void PrintPath(std::string path){
    //path.erase(0,10); //удаление странной приписки в начале (скорее всего приписка компилятора)
    cout << "Path is " << path << endl;
}

void WriteDirectoryFiles(const fs::path& MyPath){
    for (auto& p : fs::directory_iterator(MyPath)){
        cout << p.path().filename() << endl;
    }
}

bool IsDirectoryExist (string& MyPath, string& directory){
    for (auto& p : fs::directory_iterator(MyPath)){
        if(p.path().filename() == directory){
            return true;
        }
    }
    return false;
}

void CreateDirectory(const string& name, fs::path& FullPath){
    std::vector <string> PathInVec;
    string StringPath = FullPath;
    StringPath += '/' + name;
    string MyPath;
    while (StringPath.find('/') != -1){                                          //перевод всего переданного адреса в вектор,
        PathInVec.push_back(StringPath.substr(0, StringPath.find('/'))); //делавется для того, чтобы если будет укзана путь к
        StringPath.erase(0, StringPath.find('/')+1);                     //через несуществующую директорию, то она будет создана
    }
    PathInVec.push_back(StringPath);
    for (int i = 0; i < PathInVec.size()-2; ++i){
        MyPath += PathInVec[i] + '/';
        if(!IsDirectoryExist(MyPath, PathInVec[i+1])){
            fs::create_directory(MyPath + PathInVec[i+1]);
        }
    }
    MyPath += PathInVec[PathInVec.size()-2] + '/';
    if (IsDirectoryExist(MyPath, PathInVec[PathInVec.size()-1])){
        cout << "Directory " << PathInVec[PathInVec.size()-1] << " is exist" << endl;
    } else {
        fs::create_directories(MyPath+PathInVec[PathInVec.size()-1]);
        cout << "Directory "<< PathInVec[PathInVec.size()-1] <<" was created successfully" << endl;
    }
}

int main() {
    string command;
    fs::path FullPath = fs::current_path();
    while (cin >> command){
        if (command == "show_files"){
            WriteDirectoryFiles(FullPath);
        } else if(command == "print_path"){
            PrintPath(FullPath);
        } else if(command == "back"){
            FullPath.remove_filename();
            string temp;
            temp = FullPath;
            temp.erase(temp.size()-1);
            FullPath = temp;
            WriteDirectoryFiles(FullPath);
        } else if(command == "goto"){
            string floor;
            cin >> floor;
            fs::path TempPath = FullPath;
            TempPath += '/' + floor;
            try {
                WriteDirectoryFiles(TempPath);
            } catch (std::exception& e) {
                cout << e.what() << endl;
                continue;
            }
            FullPath = TempPath;
        } else if(command == "create_directory"){
            string floor;
            cin >> floor;
            CreateDirectory(floor, FullPath);
        } else if(command == "size"){
            string FileName;
            fs::path TempPath;
            cin >> FileName;
            TempPath = FullPath;
            TempPath += '/' + FileName;
            try {
                std::cout << "File size = " << fs::file_size(TempPath) << endl;
            } catch(fs::filesystem_error& e) {
                std::cout << e.what() << endl;
            }
        } else if(command == "exit"){
            break;
        } else if(command == "set_path"){
            fs::path MyPath;
            cin >> MyPath;
            fs::path TempPath;
            TempPath = "/cygdrive/";
            TempPath += MyPath;
            try {
                WriteDirectoryFiles(TempPath);

            } catch (std::exception& e) {
                cout << e.what() << endl;
                continue;
            }
            FullPath = TempPath;
        } else if(command == "copy"){
            string from;
            string to;
            cin >> from >> to;
            from.insert(0, "/cygdrive/");
            to.insert(0, "/cygdrive/"); //set_path d
            try {
                fs::copy(from, to);
                cout << "successfully" << endl;
            } catch (std::exception& e) {
                cout << e.what() << endl;
            }
        } else if(command == "delete"){
            string name;
            cin >> name;
            fs::path temp = FullPath;
            temp += '/' + name;
            fs::remove_all(temp);
            cout << "successfully" << endl;
        } else if(command == "rename"){
            string from;
            string to;
            cin >> from >> to;
            string temp = FullPath;
            temp += '/';
            fs::rename(temp+from, temp+to);
            cout << "successfully" << endl;
        } else if(command == "move_file"){
            string from;
            string to;
            cin >> from >> to; //записываю абсолютные пути файла и папки в которую он будет перемещён
            string temp = FullPath;
            temp += '/' + from;
            from = temp;
            temp = FullPath;
            temp += '/' + to;
            to = temp;
            try {
                fs::copy(from, to);
            } catch (std::exception& e) {
                cout << e.what() << endl;
                continue;
            }
            fs::remove_all(from);
            cout << "successfully" << endl;
        } else if(command == "move_directory"){
            string from;
            fs::path to;
            cout << "Введите абсолютные пути папок" << endl;
            cin >> from >> to; //записываю абсолютные пути файла и папки в которую он будет перемещён
            to = "/cygdrive" / to;
            from.insert(0, "/cygdrive/");
            CreateDirectory(from.substr(from.rfind('/')+1), to);
            try {
                fs::rename(from, to / from.substr(from.rfind('/')+1));
            } catch (std::exception& e) {
                cout << e.what() << endl;
                continue;
            }
            cout << "successfully" << endl;
        } else {
            cout << "Unknown command" << endl;
        }
    }
    return 0;
}

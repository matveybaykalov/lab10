#include <iostream>
#include <filesystem>
#include <string>
#include <sstream>
#include <exception>

namespace fs = std::filesystem;
using std::cout;
using std::cin;
using std::endl;
using std::string;

void WriteDirectoryFiles(const fs::path& MyPath){
    for (auto& p : fs::directory_iterator(MyPath)){
        cout << p.path().filename() << endl;
    }
}

bool IsDirectoryExist (const fs::path& MyPath, std::filesystem::path directory){
    for (auto& p : fs::directory_iterator(MyPath)){
        if(p.path().filename() == directory){
            return true;
        }
    }
    return false;
}

void copy (fs::path& from, fs::path& to){ //создал функцию copy, потому что библиотечная функция не копирует вложенные директории
    fs::path CopyOfFrom = to;
    CopyOfFrom.remove_filename();
    if (!IsDirectoryExist(CopyOfFrom, to.filename())){
        fs::copy(from, to);//это библиотечная функция
        for (auto &p : fs::directory_iterator(from)) {
            if (fs::is_directory(p)) {
                fs::path temp1 = from / p.path().filename();  //временные переменные использую, потому что компилятор
                fs::path temp2 = to / p.path().filename();    //выдаёт ошибку при записи  copy(from/p.path().filename(), to/p.path().filename());
                //возможно это связано с тем, что моя функция и библиотечная имеют одинаковые имена
                copy(temp1, temp2); //это моя функция
            }
        }
    } else {
        throw std::invalid_argument(to.string() + " is exist");
    }
}

int main() {
    string command;
    fs::path FullPath = fs::current_path();
    while (cin >> command){
        if (command == "show_files"){
            WriteDirectoryFiles(FullPath);
        } else if(command == "print_path"){
            cout << FullPath << endl;
        } else if(command == "exit"){
            break;
        } else if(command == "set_path"){
            fs::path TempPath;
            cin >> TempPath;
            try {
                WriteDirectoryFiles(TempPath);
            } catch (std::exception& e) {
                cout << e.what() << endl;
                continue;
            }
            FullPath = TempPath;
        } else if(command == "goto"){
            string floor;
            std::getline(cin, floor);
            floor.erase(0,1);
            fs::path TempPath = FullPath;
            TempPath += '\\' + floor; //временный путь нужен для проверки корректности
            try {
                WriteDirectoryFiles(TempPath);
            } catch (std::exception& e) {
                cout << e.what() << endl;
                continue;
            }
            FullPath = TempPath;
        } else if(command == "back"){
            FullPath.remove_filename();
            string temp;
            std::stringstream ss; //создаю поток, чтобы перевести path в string. После чего удалить два поледних
            ss << FullPath;       //слеша, который остались после remove_filename()
            ss >> temp;
            temp.erase(temp.size()-3);
            temp.erase(0, 1);
            while (temp.find("\\\\") != -1){                         //удаляю экранированные слеши из строчки, потому что
                temp.replace(temp.find("\\\\"), 2, "\\"); //при присваивании FullPath = temp слеши дополнительно экранируются
            }
            FullPath = temp;
            WriteDirectoryFiles(FullPath);
        } else if(command == "create_directory"){
            string floor;
            std::getline(cin,floor);
            floor.erase(0, 1);
            if (!(IsDirectoryExist(FullPath, floor))){
                fs::path TempPath = FullPath;
                TempPath += '\\' + floor;
                try {
                    fs::create_directory(TempPath);
                    cout << "successfully" << endl;
                } catch (std::exception &e) {
                    cout << e.what() << endl;
                    continue;
                }
                FullPath = TempPath;
            } else {
                cout << floor << " is exist" << endl;
            }
        } else if(command == "size"){
            string FileName;
            fs::path TempPath;
            std::getline(cin, FileName);
            FileName.erase(0, 1);
            TempPath = FullPath;
            TempPath += '\\' + FileName;
            try {
                std::cout << "File size = " << fs::file_size(TempPath) << endl;
            } catch(fs::filesystem_error& e) {
                std::cout << e.what() << endl;
            }
        } else if(command == "copy"){ //при копировании нужно указывать абсолютные пути файлов
            fs::path from, to;
            cin >> from >> to;
            try {
                copy(from, to);
                cout << "successfully" << endl;
            } catch (std::exception &e) {
                cout << e.what() << endl;
            }
        } else if(command == "delete"){
            string name;
            cin >> name;
            fs::path temp = FullPath;
            temp += '\\' + name;
            fs::remove_all(temp);
            cout << "successfully" << endl;
        } else if(command == "rename"){
            string from;
            string to;
            cin >> from >> to;
            try {
                fs::rename(FullPath/from, FullPath/to);
                cout << "successfully" << endl;
            } catch (std::exception &e) {
                cout << e.what() << endl;
            }
        } else if(command == "move"){
            fs::path from, to;
            cin >> from >> to; //считываю абсолютные пути файла и папки в которую он будет перемещён
            to /= from.filename();
            try {
                fs::copy(from, to);
            } catch (std::exception& e) {
                cout << e.what() << endl;
                continue;
            }
            fs::remove_all(from);
            cout << "successfully" << endl;
        } else {
            cout << "Unknown command" << endl;
        }
    }
    return 0;
}

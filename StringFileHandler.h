#pragma once

#include <string>
#include <fstream>
#include <algorithm>

class StringFileHandler {
public:
    StringFileHandler(std::string path) : _path(std::move(path)), infile(_path) {}

    [[nodiscard]] std::string GetPath() const {
        return _path;
    }

    std::string GetLine() {
        std::string line;
        std::getline(infile, line);
        return line;
    }

    std::string GetLineWithNextLine() {
        std::ostringstream line;
        int c;
        while (infile.good() && !infile.eof()){
            c = infile.get();
            if(c == 0xff){
                continue;
            }
            line << (char)c;
            if(c == '\n'){
                break;
            }
        }
        return line.str();
    }

    bool HasNext() const {
        return infile.good();
    }


private:
    std::string _path;
    std::ifstream infile;

};

#pragma once

#include <boost/filesystem.hpp>
#include <map>
#include <mutex>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include <regex>
#include <cmath>
#include <sstream>
#include <iomanip>


namespace FlowFile {

    inline std::map<std::string, std::mutex> _locks;

    inline void lockFs(const std::string &name) {
        _locks[name].lock();
    }

    inline void unlockFs(const std::string &name) {
        _locks[name].unlock();
    }

    inline bool deleteByPath(std::string path) {
        boost::filesystem::remove_all(path);
        return true;
    }

    inline bool deleteFile(std::string cs) {
        boost::filesystem::remove(cs);
        return true;
    }

    inline std::string combinePath(std::string path1, std::string path2) {
        boost::filesystem::path path(path1);
        path /= path2;
        return path.string();
    }

    inline bool createDirIfNotExist(std::string path, bool assumeFile = false) {
        if ((path == "/" || path == "./") ||
            (path.find("/") == std::string::npos && path.find("\\") == std::string::npos)) {
            return false;
        }

        boost::filesystem::path dir(path);
        if (assumeFile) {
            dir = dir.parent_path();
            auto newPath = dir.string();
            if (newPath == "/" || newPath == "./") {
                return false;
            }
        }

        if (boost::filesystem::create_directories(dir)) {
            return true;
        }
        return false;
    }

    inline bool fileExist(const std::string &path) {
        boost::filesystem::path p(path);
        return boost::filesystem::exists(p) && boost::filesystem::is_regular_file(p);
    }

    inline std::vector<unsigned char> loadBytes(const std::string &filename) {
        if (!fileExist(filename)) {
            throw 404;
        }
        // open the file:
        std::streampos fileSize;
        std::ifstream file(filename, std::ios::binary);

        // get its size:
        file.seekg(0, std::ios::end);
        fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        // read the data:
        std::vector<unsigned char> fileData(fileSize);
        file.read(reinterpret_cast<char *>(&fileData[0]), fileSize);
        return fileData;
    }

    inline std::string getCurrentDirectory() {
        return boost::filesystem::current_path().string();
    }

    inline bool isDirectory(const std::string &path) {
        boost::filesystem::path p(path);
        return boost::filesystem::exists(p) && boost::filesystem::is_directory(p);
    }

    inline void writeBinaryVector(const std::string &file, const std::vector<unsigned char> &data) {
        createDirIfNotExist(file, true);
        std::ofstream of(file, std::ios::out | std::ios::binary);
        std::ostream_iterator<unsigned char> ofitr(of);
        copy(data.begin(), data.end(), ofitr);
    }

    inline void appendToFile(const std::string &file, const std::string &value) {
        std::ofstream out(file, std::ios::app);
        out.seekp(0, std::ios::end);
        if (out.tellp() == 0)
            out << value;
        else
            out << std::endl << value;
    }

    inline std::string getFilename(const std::string &file) {
        namespace fs = boost::filesystem;
        return fs::path(file).filename().string();
    }

    inline std::string getFileFolder(const std::string &path) {
        namespace fs = boost::filesystem;
        auto fs_path = fs::path(path).parent_path();
        return fs_path.string();
    }

    inline std::string getTempFolder() {
        return boost::filesystem::temp_directory_path().string();
    }

    inline std::string getTempFolder(const std::string &append) {
        return combinePath(getTempFolder(), append);
    }

    inline std::vector<std::string> getFilesInDirectory(std::string path, std::string filter = "") {
        namespace fs = boost::filesystem;
        std::vector<std::string> files;
        if (!fs::is_directory(path)) return files;

        std::regex e(filter);

        fs::path root(path);
        fs::directory_iterator it_end;
        for (fs::directory_iterator it(root); it != it_end; ++it) {
            if (fs::is_regular_file(it->path())) {
                if (filter.empty() || std::regex_match(it->path().string(), e, std::regex_constants::match_any))
                    files.push_back(it->path().string());
            }
        }

        return files;
    }

    inline std::vector<std::string> getFoldersInDirectory(std::string path, std::string filter = "") {
        namespace fs = boost::filesystem;
        std::vector<std::string> files;
        if (!fs::is_directory(path)) return files;

        fs::path root(path);
        fs::directory_iterator it_end;
        for (fs::directory_iterator it(root); it != it_end; ++it) {
            if (fs::is_directory(it->path())) {
                if (filter.empty() || it->path().string().find(filter) != std::string::npos)
                    files.push_back(it->path().string());
            }
        }
        return files;
    }

    inline std::string fileToString(std::string file) {
        std::ifstream t(file);
        std::string rtn;

        t.seekg(0, std::ios_base::end);
        rtn.reserve((unsigned int) t.tellg());
        t.seekg(0, std::ios_base::beg);

        rtn.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

        t.close();
        return rtn;
    }

    inline void appendVectorToFile(const std::string &file, const std::vector<std::string> &lines) {
        lockFs(file);
        std::ofstream myfile;
        myfile.open(file, std::ios_base::out | std::ios_base::app);
        for (std::string l : lines) myfile << l << std::endl;
        myfile.close();
        unlockFs(file);
    }

    inline void stringVectorToFile(const std::string &file, const std::vector<std::string> &lines) {
        std::ofstream myfile;
        myfile.open(file);
        for (std::string l : lines) myfile << l << std::endl;
        myfile.close();
    }

    inline void
    stringVectorToFile(const std::string &file, const std::vector<std::string> &lines, const std::string &seperator) {
        std::ofstream myfile;
        myfile.open(file);
        for (std::string l : lines) myfile << l << seperator;
        myfile.close();
    }

    inline std::vector<std::string> fileToStringVector(const std::string &file) {
        if (!boost::filesystem::is_regular_file(file)) {
            return std::vector<std::string>();
        }

        std::ifstream myfile(file);
        std::vector<std::string> rtn;

        std::string line;
        if (myfile.is_open()) {
            while (getline(myfile, line)) {
                if (!line.empty())
                    rtn.push_back(line);
            }
            myfile.close();
        }

        return rtn;
    }

    inline std::vector<std::string> findFiles(const std::string &path, const std::string &pattern = "") {
        std::vector<std::string> rtn;

        if (!boost::filesystem::exists(boost::filesystem::path(path))) {
            return rtn;
        }

        std::smatch m;
        std::regex e(pattern);

        auto end = boost::filesystem::recursive_directory_iterator();
        for (auto it = boost::filesystem::recursive_directory_iterator(path); it != end; ++it) {
            if (boost::filesystem::is_regular_file(it->path())) {
                std::string realPath = boost::filesystem::canonical(it->path()).string();
                if (pattern.empty() || regex_search(realPath, m, e))
                    rtn.emplace_back(realPath);
            }
        }

        return rtn;
    }

    inline void replaceAll(std::string &str, const std::string &from, const std::string &to) {
        if (from.empty()) return;
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
    }

    inline std::string copyFile(std::string file, const std::string &to, const std::string &removeFromPath = "") {
        replaceAll(file, removeFromPath, "");
        std::string copyTo = combinePath(to, file);
        createDirIfNotExist(copyTo, true);
        boost::filesystem::copy_file(file, copyTo, boost::filesystem::copy_option::overwrite_if_exists);
        return copyTo;
    }

    inline std::string getFileExtension(const std::string &file) {
        return boost::filesystem::extension(file);
    }

    inline time_t getLastModified(const std::string &file) {
        return boost::filesystem::last_write_time(file);
    }

    inline void mv(const std::string &from, const std::string &to) {
        createDirIfNotExist(to, true);
        boost::filesystem::rename(from, to);
    }

    inline std::size_t getFileSize(const std::string &file) {
        return boost::filesystem::file_size(file);
    }

    inline std::string byteToHumanReadable(const std::size_t &bytes) {
        float rtn = static_cast<float>(bytes);
        const std::string endings[] = {"B", "KB", "MB", "GB", "PB"};
        const size_t entries = 5;
        size_t pos = 0;
        while (rtn > 9999 && pos < entries) {
            rtn /= 1024;
            ++pos;
        }
        std::stringstream rtnStream;
        rtnStream << std::defaultfloat << std::setprecision(5) << rtn << " " << endings[pos];

        return rtnStream.str();
    }
};

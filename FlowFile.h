#pragma once

#ifdef _WIN32
#include <boost/filesystem.hpp>
#endif

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
#include <filesystem>
#include <cstdlib>
#include <chrono>
#include <fstream>


namespace FlowFile {

    inline std::unordered_map<std::string, std::mutex> _locks;

    inline void lockFs(const std::string &name) {
        _locks[name].lock();
    }

    inline void unlockFs(const std::string &name) {
        _locks[name].unlock();
    }

    inline bool deleteByPath(std::string path) {
        std::filesystem::remove_all(path);
        return true;
    }

    inline bool deleteFolder(std::string path) {
        std::error_code ec;
        std::filesystem::remove_all(path, ec);
        return !ec;
    }

    inline bool deleteFile(std::string path) {
        std::error_code ec;
        std::filesystem::remove(path);
        return !ec;
    }

    inline std::string combinePathLinux(std::string path1, std::string path2) {
        const bool slashFirst = path1.ends_with('/') || path1.ends_with('\\');
        const bool slashSecond = path2.starts_with('/') || path2.starts_with('\\');
        if (slashFirst && slashSecond) {
            return path1 + path2.substr(1);
        }
        if (slashFirst || slashSecond) {
            return path1 + path2;
        }
        return path1 + '/' + path2;
    }

    inline std::string normalizePath(const std::string path) {
        return std::filesystem::weakly_canonical(path).string();
    }


    inline std::string homeFolder() {
        char *pPath;
        pPath = getenv("HOME");
        if (pPath == NULL) {
            pPath = getenv("USERPROFILE");
            if (pPath == NULL) {
                pPath = getenv("HOMEPATH");
                if (pPath == NULL) {
                    return "";
                }
            }
        }
        return normalizePath(std::string(pPath));
    }

    inline std::string combinePath(const std::string &path1, const std::string &path2) {
        const bool slashFirst = path1.ends_with('/') || path1.ends_with('\\');
        const bool slashSecond = path2.starts_with('/') || path2.starts_with('\\');
        if (slashFirst && slashSecond) {
            return path1 + path2.substr(1);
        }
        if (slashFirst || slashSecond) {
            return path1 + path2;
        }
#ifdef _WIN32
        return  path1 +'\\' + path2;
#else
        return path1 + '/' + path2;
#endif
    }

    inline bool createDirIfNotExist(const std::string &path, bool assumeFile = false) {
        if ((path.empty() || path == "/" || path == "./") ||
            (assumeFile && path.find("/") == std::string::npos && path.find("\\") == std::string::npos)) {
            return false;
        }

        std::filesystem::path dir(path);
        if (assumeFile) {
            dir = dir.parent_path();
            auto newPath = dir.string();
            if (newPath == "/" || newPath == "./") {
                return false;
            }
        }

        if (std::filesystem::create_directories(dir)) {
            return true;
        }
        return false;
    }

    inline std::string getParentPath(const std::string &path) {
        std::filesystem::path current_path(path);
        return current_path.parent_path().string();
    }

    inline std::string getName(const std::string path) {
        std::filesystem::path current_path(path);
        if (is_directory(current_path))
            return current_path.filename().string();
        return current_path.filename().string();
    }

    inline bool exists(const std::string &path) {
        if (path.empty())
            return false;
        std::filesystem::path p(path);
        return std::filesystem::exists(p);
    }

    inline bool fileExist(const std::string &path) {
        std::filesystem::path p(path);
        return std::filesystem::exists(p) && std::filesystem::is_regular_file(p);
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
        return std::filesystem::current_path().string();
    }

    inline bool isDirectory(const std::string &path) {
        std::filesystem::path p(path);
        return std::filesystem::exists(p) && std::filesystem::is_directory(p);
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
        return std::filesystem::path(file).filename().string();
    }

    inline std::string getParentFolder(const std::string &path) {
        namespace fs = std::filesystem;
        auto fs_path = fs::path(path).parent_path();
        return fs_path.string();
    }

    inline std::string getTempFolder() {
        return std::filesystem::temp_directory_path().string();
    }

    inline std::string getTempFolder(const std::string &append) {
        return combinePath(getTempFolder(), append);
    }

    inline std::vector<std::string> getContentOfDirectory(const std::string &path, std::string filter = "") {
        namespace fs = std::filesystem;
        std::vector<std::string> content;
        if (!fs::is_directory(path)) return content;

        std::regex e(filter);

        fs::path root(path);
        fs::directory_iterator it_end;
        for (fs::directory_iterator it(root); it != it_end; ++it) {
            content.push_back(it->path().string());
        }

        return content;
    }

    inline std::vector<std::string> getFilesInDirectory(const std::string &path, std::string filter = "") {
        namespace fs = std::filesystem;
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
        namespace fs = std::filesystem;
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

    inline void stringToFile(const std::string &file, const std::string &text) {
        createDirIfNotExist(file, true);
        std::ofstream myfile;
        myfile.open(file);
        myfile << text;
        myfile.close();
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
        if (!std::filesystem::is_regular_file(file)) {
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

        if (!std::filesystem::exists(std::filesystem::path(path))) {
            return rtn;
        }

        std::smatch m;
        std::regex e(pattern);

        auto end = std::filesystem::recursive_directory_iterator();
        for (auto it = std::filesystem::recursive_directory_iterator(path); it != end; ++it) {
            if (std::filesystem::is_regular_file(it->path())) {
                std::string realPath = std::filesystem::canonical(it->path()).string();
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
        std::filesystem::copy_file(file, copyTo, std::filesystem::copy_options::overwrite_existing);
        return copyTo;
    }

    inline std::string getFileExtension(const std::string &file) {
#ifdef _WIN32
        return boost::filesystem::extension(file);
#else
        return std::filesystem::path(file).extension();
#endif
    }

    inline std::filesystem::file_time_type getLastWriteTime(const std::string &file) {
        return std::filesystem::last_write_time(std::filesystem::path(file));
    }

    inline time_t getLastModified(const std::string &file) {
#ifdef _WIN32
        return boost::filesystem::last_write_time(file);
#else
        const auto glwt = getLastWriteTime(file);
        auto cftime = std::chrono::system_clock::to_time_t(std::chrono::file_clock::to_sys(glwt));
        return cftime;
#endif
    }

    inline void mv(const std::string &from, const std::string &to) {
        createDirIfNotExist(to, true);
        const auto full_new_path = normalizePath(to);
        std::error_code ec;
        std::filesystem::rename(from, full_new_path, ec);
        if (ec.value() == 18) {
            std::filesystem::copy(from, to, ec);
            if (!ec) {
                std::filesystem::remove_all(from);
            }
        }
    }

    inline void mvFolder(const std::string &old_path, const std::string &new_path) {
        const auto full_new_path = normalizePath(new_path);
        std::error_code ec;
        std::filesystem::rename(old_path, full_new_path, ec);
        if (ec.value() == 18) {
            std::filesystem::copy(old_path, new_path, ec);
            if (!ec) {
                std::filesystem::remove_all(old_path);
            }
        }
    }

    inline std::size_t getFileSize(const std::string &file) {
        return std::filesystem::file_size(file);
    }

    inline void pathNameSanitize(std::string &name, const char changeTo = ' ') {
        static const std::string illegalChars = "*.\"/\\[]:;|,-";
        auto pos = name.find_first_of(illegalChars);
        while (pos != std::string::npos) {
            name[pos] = changeTo;
            pos = name.find_first_of(illegalChars, pos + 1);
        }
    }

    inline std::string sanitizePathName(std::string name, const char changeTo = ' ') {
        pathNameSanitize(name, changeTo);
        return name;
    }

    inline std::string byteToHumanReadable(const std::size_t &bytes) {
        float rtn = static_cast<float>(bytes);
        const std::string endings[] = {"B", "KB", "MB", "GB", "PB"};
        const size_t entries = 4;
        size_t pos = 0;
        while (rtn > 999 && pos < entries) {
            rtn /= 1024;
            ++pos;
        }
        std::stringstream rtnStream;
        rtnStream << std::defaultfloat << std::setprecision(5) << rtn << " " << endings[pos];

        return rtnStream.str();
    }

};

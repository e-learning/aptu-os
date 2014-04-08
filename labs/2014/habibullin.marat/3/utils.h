#ifndef UTILS_H
#define UTILS_H

#include <ctime>
#include <string>
#include <sys/stat.h>

using std::string;

class Utils {
public:
    Utils();

    static string CurrentDatetime() {
        time_t now = time(0);
        struct tm local_time = *localtime(&now);
        char time_as_array[80];
        strftime(time_as_array, sizeof(time_as_array), "%Y-%m-%d.%X", &local_time);
        return time_as_array;
    }

    static string FileModTime(string const& path) {
        struct tm local_time;
        struct stat attrs;
        stat(path.c_str(), &attrs);
        local_time = *localtime(&(attrs.st_mtime));
        char time_as_array[80];
        strftime(time_as_array, sizeof(time_as_array), "%Y-%m-%d.%X", &local_time);
        return time_as_array;
    }

    static string CutPathHead(string& path) {
        if(path.empty()) {
            return "";
        }
        if(path.at(0) == '/') {
            path = path.substr(1, path.size());
        }
        size_t delim_pos = path.find('/');
        if(delim_pos == string::npos) {
            string head = path;
            path = "";
            return head;
        }
        string head = path.substr(0, delim_pos);
        path = path.substr(delim_pos, path.size());
        return head;
    }

    static string CutPathTail(string& path) {
        if(path.empty()) {
            return "";
        }
        if(path.at(path.size() - 1) == '/') {
            path = path.substr(0, path.size() - 1);
        }
        size_t delim_pos = path.find_last_of('/');
        if(delim_pos == string::npos) {
            string tail = path;
            path = "";
            return tail;
        }
        string tail = path.substr(delim_pos + 1, path.size());
        path = path.substr(0, delim_pos);
        return tail;
    }

    static string PathWithNoFileName(string const& path) {
        size_t delim_pos = path.find_last_of('/');
        if(delim_pos == 0) {
            return path.substr(0, 1);
        }
        return path.substr(0, delim_pos);
    }

    static string GetPathTail(string const& path) {
        size_t delim_pos = path.find_last_of('/');
        if(delim_pos == string::npos) {
            return path;
        }
        return path.substr(delim_pos + 1, path.size());
    }

    static string ConstructPath(string const& root, size_t block) {
        string path = (root.at(root.size() - 1) == '/') ?
                    root + std::to_string(block) :
                    root + '/' + std::to_string(block);
        return path;
    }

    static size_t CountPathUnits(string const& path) {
        if(path.empty() || path == "/") {
            return 0;
        }
        string pathcpy = path;
        if(pathcpy.at(0) == '/') {
            pathcpy = pathcpy.substr(1, pathcpy.size());
        }
        if(pathcpy.at(pathcpy.size() - 1) == '/') {
            pathcpy = pathcpy.substr(0, pathcpy.size() - 1);
        }
        size_t count = 1;
        size_t delim_pos = 0;
        while((delim_pos = pathcpy.find('/', delim_pos)) != string::npos) {
            ++count;
            ++delim_pos;
        }
        return count;
    }

    static size_t SizeTMax() { return (size_t) -1; }

    static string ToRawString(string const& str, size_t raw_size) {
        string rawstr;
        size_t i = 0;
        while(i != raw_size && i != str.size()) {
            rawstr.push_back(str.at(i));
            ++i;
        }
        while(i != raw_size) {
            rawstr.push_back('\n');
            ++i;
        }
        return rawstr;
    }

    static string FromRawString(string const& rawstr) {
        size_t pos = rawstr.find('\n');
        if(pos == string::npos) {
            return rawstr;
        }
        return rawstr.substr(0, pos);
    }

    static bool IsParentDir(string parent, string child) {
        if(parent != "/") {
            return parent == PathWithNoFileName(child);
        }
        return CountPathUnits(child) == 1;
    }

    static int ComparePathsNesting(string path1, string path2) {
        while(!path1.empty() && !path2.empty()) {
            if(CutPathHead(path1) != CutPathHead(path2)) {
                return 0;
            }
        }
        return path1.empty() ? -1 : 1 ;
    }

    static string NormalizePath(string const& path) {
        string normalized = path;
        if(normalized != "/") {
            if(normalized.at(normalized.size() - 1) == '/') {
                normalized = normalized.substr(0, normalized.size() - 1);
            }
        }
        return normalized;
    }
};

#endif // UTILS_H

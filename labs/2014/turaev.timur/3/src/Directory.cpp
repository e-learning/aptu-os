#include <sstream>
#include "Directory.hpp"

using std::endl;

string Directory::get_info()
{
    std::stringstream ss;
    for (auto it = directories.begin(); it != directories.end(); ++it) {
        ss << "D " << it->first << endl;
    }
    for (auto it = files.begin(); it != files.end(); ++it) {
        ss << it->second.get_info() << endl;
    }
    return ss.str();
}

void Directory::load(ifstream &s)
{
    s >> name;
    s >> modified_time;
    int d, f;
    s >> d >> f;
    for (int i = 0; i < d; i++) {
        Directory directory;
        directory.load(s);
        directories[directory.name] = directory;
    }
    for (int i = 0; i < f; i++) {
        File file;
        file.load(s);
        files[file.getName()] = file;
    }
}

void Directory::save(ofstream &s) const
{
    s << name << " ";
    s << modified_time << " ";
    s << directories.size() << " " << files.size() << endl;
    for (auto it = directories.begin(); it != directories.end(); ++it) {
        it->second.save(s);
    }
    for (auto it = files.begin(); it != files.end(); ++it) {
        it->second.save(s);
    }
}

Directory *Directory::findLastDirectory(Path const &path)
{
    Directory *current_dir = this;

    vector<string> const &t = path.getSplittedPath();

    for (int i = 0; i < (int)t.size() - 1; ++i) {
        if (directories.find(t[i]) == directories.end())
            return nullptr;
        current_dir = &directories[t[i]];
    }
    return current_dir;
}

#include <string>
#include <vector>

using std::string;
using std::vector;


typedef void (*CommandFunc)(vector<string> const &);

void ps(vector<string> const &);
void pwd(vector<string> const &);
void ls(vector<string> const &);
void kill(vector<string> const &);

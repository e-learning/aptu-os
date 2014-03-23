#include <fstream>
#include <string>
using namespace std;

/**
 * Usage: codegen <prog> <args_size> [<arg_name>, ...]
 */
int main(int argc, const char *argv[])
{
    if (argc < 4)
        return 1;

    const char *prog_name = argv[1];
    const char *read_metadata = argv[2][0] == 't' ? "true" : "false";
    int args_size = stoi(argv[3]);

    if (argc < 4+args_size)
        return 2;

    string usage_str = string("Usage: ") + prog_name + string(" ");
    for (int i = 0; i < args_size; i++) {
        usage_str += string("<") + argv[4+i] + string("> ");
    }

    string call_str = string("fs.cmd_") + prog_name + string("(");
    if (args_size > 1) {
        for (int i = 1; i < args_size; i++) {
            call_str += "argv[" + to_string(i);
            if (i == args_size-1)
                call_str += "]);";
            else
                call_str += "], ";
        }
    } else {
        call_str += ");";
    }

    fstream out(string(prog_name) + ".cpp", fstream::out | fstream::trunc);

    out << "#include <iostream>" << endl;
    out << "using namespace std;" << endl;
    out << "#include \"filesystem.hpp\"" << endl;
    out << endl;
    out << "int main(int argc, const char *argv[]) {" << endl;
    out << "    argc--;" << endl;
    out << "    argv++;" << endl;
    out << "    if (argc < " << args_size << ") {" << endl;
    out << "        cerr << \"" << usage_str << "\" << endl;" << endl;
    out << "        return 1;" << endl;
    out << "    }" << endl;
    out << endl;
    out << "    try {" << endl;
    out << "        filesystem fs(argv[0]," << read_metadata << ");" << endl;
    out << "        " << call_str << endl;
    out << "    } catch (filesystem::error e) {" << endl;
    out << "        cerr << \"Error: \" << e.what() << endl;" << endl;
    out << "        return 1;" << endl;
    out << "    }" << endl;
    out << "    return 0;" << endl;
    out << "}" << endl;

    return 0;
}

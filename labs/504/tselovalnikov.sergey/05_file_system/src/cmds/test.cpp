#include "../FileSystem.h"

int main(int argc, char *argv[]) {
    string path = argv[1];
    Context *ctx = Context::readFromDirectory(path);

    try {
        FileSystem::init(ctx);
        FileSystem fs(ctx);
        fs.import_file("/tmp/fs/config", "/b");
        fs.file_info("/b");
        fs.move("/b", "/bb");
        fs.import_file("/tmp/fs/config3", "/sec");
        fs.file_info("/sec");
        fs.export_file("/bb", "/tmp/fs/config2");
        fs.export_file("sec", "/tmp/fs/config4");
        fs.rm("/sec");
        fs.mkdir("/dir1");
        fs.mkdir("/dir1/dir2");
        fs.import_file("/tmp/fs/config", "/dir1/hello");
        fs.file_info("/dir1");
        fs.copy("/dir1", "/dir3");
        fs.file_info("/dir3");
        cout << endl;
        for(int i = 0; i < 40; i++) {
            fs.import_file("/tmp/fs/config", "/dir3/hello" + to_string(i));
        }
        fs.file_info("/dir3");

        fs.rm("/dir1");


        //
        cout << "****new****" << endl;
        FileSystem fs2(ctx);
        fs.file_info("/dir3");
    } catch (runtime_error& e) {
        cerr << e.what() << endl;
    }

    return 0;
}
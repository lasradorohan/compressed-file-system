#ifndef INTERACTIVEPROMPT_H
#define INTERACTIVEPROMPT_H

#include "filemanager.h"

enum command { help, listfiles, listprops, makefile, writefile, readfile, makefolder, del, ren, find, impfile, expfile, cd, tree, quit, err };

class InteractivePrompt{
    FileManager fm;
    FileManager::ErrorCode flag;

    std::vector<std::string> read_tokenize(std::istream& in);    
    command recognizeToken(std::string s);
    void execute_help(std::string cmd);
    void execute_listfiles();
    void execute_listprops();
    void execute_cd(std::string fname);
    void execute_makeEntity(std::string fname, EntityType et);
    void execute_writeFile(std::string fname);
    void execute_readFile(std::string fname);
    void execute_del(std::string fname);
    void execute_rename(std::string fname, std::string newfname);
    void execute_find(std::string rgxstr);
    void execute_impfile(std::string fname);
    void execute_expfile(std::string fname);
    void execute_printTree();
    
    public:

    InteractivePrompt(FileManager& fm);

    void welcome(std::ostream& out);
    void promptLoop();;
};

#endif
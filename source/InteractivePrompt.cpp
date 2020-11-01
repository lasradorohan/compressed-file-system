#include "InteractivePrompt.h"
#include "maketable.cpp"
#include <iostream>
#include <filesystem>


std::vector<std::string> InteractivePrompt::read_tokenize(std::istream& in){
    std::vector<std::string> tokens;
    std::string temp;
    do{
        in>>temp;
        tokens.push_back(temp);
        while(in.peek() == ' ') in.ignore();
    }while(in.peek()!='\n');
    in.ignore();
    return tokens;
}

command InteractivePrompt::recognizeToken(std::string s){
    if(s.compare("help")==0){
        return command::help;
    } else if(s.compare("listfiles")==0){
        return command::listfiles;
    } else if(s.compare("listprops")==0){
        return command::listprops;
    } else if(s.compare("makefile")==0){
        return command::makefile;
    } else if(s.compare("writefile")==0){
        return command::writefile;
    } else if(s.compare("readfile")==0){
        return command::readfile;
    } else if(s.compare("makefolder")==0){
        return command::makefolder;
    } else if(s.compare("delete")==0){
        return command::del;
    } else if(s.compare("rename")==0){
        return command::ren;
    } else if(s.compare("find")==0){
        return command::find;
    } else if(s.compare("import")==0){
        return command::impfile;
    } else if(s.compare("export")==0){
        return command::expfile;
    } else if(s.compare("cd")==0){
        return command::cd;
    } else if(s.compare("tree")==0){
        return command::tree;
    } else if(s.compare("exit")==0){
        return command::quit;
    } else {
        return command::err;
    }
}

void InteractivePrompt::execute_help(std::string cmd = ""){
    std::ifstream in;
    if(cmd.size()==0){
        in.open("./help/help.txt");
    } else {
        switch (recognizeToken(cmd)){
            case command::help: in.open("./help/help_help.txt"); break;
            case command::listfiles: in.open("./help/help_listfiles.txt"); break;
            case command::listprops: in.open("./help/help_listprops.txt"); break;
            case command::makefile: in.open("./help/help_makefile.txt"); break;
            case command::writefile: in.open("./help/help_writefile.txt"); break;
            case command::readfile: in.open("./help/help_readfile.txt"); break;
            case command::makefolder: in.open("./help/help_makefolder.txt"); break;
            case command::del: in.open("./help/help_delete.txt"); break;
            case command::ren: in.open("./help/help_rename.txt"); break;
            case command::find: in.open("./help/help_find.txt"); break;
            case command::impfile: in.open("./help/help_import.txt"); break;
            case command::expfile: in.open("./help/help_export.txt"); break;
            case command::cd: in.open("./help/help_cd.txt"); break;
            case command::tree: in.open("./help/help_tree.txt"); break;
            case command::quit: in.open("./help/help_exit.txt"); break;
        }
    }
    std::cout<<std::endl<<std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>())<<std::endl<<std::endl;
}

void InteractivePrompt::execute_listfiles(){
    std::vector<std::string> filelist = fm.listFiles();
    std::copy(filelist.begin(), filelist.end(), std::ostream_iterator<std::string>(std::cout, "\n"));
}

void InteractivePrompt::execute_listprops(){
    std::vector<std::vector<std::string>> pm = fm.listPropsMat();
    if(pm.size()==0) std::cout<<"Folder Empty!"<<std::endl;
    else maketable(std::cout, std::vector<std::string>{"Name", "Type", "File Size", "Size on Disk", "Ratio"}, fm.listPropsMat(), std::vector<int>{20, 10, 12, 14, 12});
}

void InteractivePrompt::execute_cd(std::string fname){
    flag = fm.changeDir(fname);
    if(flag == FileManager::ErrorCode::FolderNotFound) 
        std::cout<<"Folder \'"<<fname<<"\' not found"<<std::endl;
}

void InteractivePrompt::execute_makeEntity(std::string fname, EntityType et){
    if(et == EntityType::folder) flag = fm.makefolder(fname);
    else if(et == EntityType::file) flag = fm.makefile(fname);
    if(flag==FileManager::ErrorCode::FileFolderExists) std::cout<<"File/Folder with name \'"<<fname<<"\' already exists"<<std::endl;
}

void InteractivePrompt::execute_writeFile(std::string fname){
    if(!fm._fileExists(fm.currentFolder, fname, EntityType::file)){
        std::cout<<"File with name \'"<<fname<<"\' doesn\'t exist"<<std::endl;
        return;
    }
    std::string temp;
    std::stringstream ss;
    std::cout<<"Enter content to add(write --eof-- on new line to end):"<<std::endl;
    while(true){
        getline(std::cin, temp, '\n');
        if(temp.compare("--eof--") == 0) break;
        ss<<temp<<'\n';
    }
    std::string content = ss.str();
    fm.writeFile(fname, content, true);
}

void InteractivePrompt::execute_readFile(std::string fname){
    std::string buffer;
    flag = fm.readFile(fname, buffer);
    if(flag==FileManager::ErrorCode::FileNotFound){
        std::cout<<"File with name \'"<<fname<<"\' doesn\'t exist"<<std::endl;
    } else {
        std::cout<<std::endl<<buffer<<std::endl;
    }
}

void InteractivePrompt::execute_del(std::string fname){
    flag = fm.del(fname);
    if(flag==FileManager::ErrorCode::FileFolderNotFound) std::cout<<"File/Folder with name \'"<<fname<<"\' not found"<<std::endl;
}

void InteractivePrompt::execute_rename(std::string fname, std::string newfname){
    flag = fm.rename(fname, newfname);
    if(flag==FileManager::ErrorCode::FileFolderExists) std::cout<<"File/Folder with name \'"<<newfname<<"\' already exists"<<std::endl;
    if(flag==FileManager::ErrorCode::FileNotFound) std::cout<<"File \'"<<fname<<"\' not found"<<std::endl;
}
void InteractivePrompt::execute_find(std::string rgxstr){
    std::string foundpath;
    flag = fm.find(rgxstr, foundpath);
    if(flag==FileManager::ErrorCode::FileFolderNotFound){
        std::cout<<"Could not find match for "<<rgxstr<<std::endl;
    } else {
        std::cout<<"File matching "<<rgxstr<<" found at: ./"<<foundpath<<std::endl;
    } 
}

void InteractivePrompt::execute_impfile(std::string fname){
    if(!std::filesystem::exists("./import/"+fname)){
        std::cout<<"Could not find file \'"<<fname<<"\' in import folder"<<std::endl;
    } else {
        flag = fm.importfile(fname);
        if(flag == FileManager::FileFolderExists) std::cout<<"File/Folder with name \'"<<fname<<"\' already exists"<<std::endl;
    }
}

void InteractivePrompt::execute_expfile(std::string fname){
    flag = fm.exportfile(fname);
    if(flag==FileManager::ErrorCode::FileNotFound) std::cout<<"File with name \'"<<fname<<"\' doesn\'t exist"<<std::endl;
    if(flag==FileManager::ErrorCode::IsFolder) std::cout<<fname<<" is a folder"<<std::endl;
}

void InteractivePrompt::execute_printTree(){ fm.printTree(std::cout); }

InteractivePrompt::InteractivePrompt(FileManager& fm): fm(fm){}

void InteractivePrompt::welcome(std::ostream& out){
    std::ifstream in("./help/welcome.txt");
    out<<std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>())<<std::endl<<std::endl;
}

void InteractivePrompt::promptLoop(){
    std::string input;
    command lastCommand = command::err;
    while(lastCommand != quit){
        fm.printPromptString(std::cout);
        std::vector<std::string> tokens = read_tokenize(std::cin);
        lastCommand = recognizeToken(tokens[0]);
        bool usageError = false;
        switch(lastCommand){
            case command::help:
                if(tokens.size()>1) execute_help(tokens[1]);
                else execute_help();
                break;
            case command::listfiles:
                if(tokens.size()!=1){usageError = true; break;}
                execute_listfiles();
                break;
            case command::listprops:
                if(tokens.size()!=1){usageError = true; break;}
                execute_listprops();
                break;
            case command::makefile:
                if(tokens.size()!=2){usageError = true; break;}
                execute_makeEntity(tokens[1], EntityType::file);
                break;
            case command::writefile:
                if(tokens.size()!=2){usageError = true; break;}
                execute_writeFile(tokens[1]);
                break;
            case command::readfile:
                if(tokens.size()!=2){usageError = true; break;}
                execute_readFile(tokens[1]);
                break;
            case command::makefolder:
                if(tokens.size()!=2){usageError = true; break;}
                execute_makeEntity(tokens[1], EntityType::folder);
                break;
            case command::del:
                if(tokens.size()!=2){usageError = true; break;}
                execute_del(tokens[1]);
                break;
            case command::ren:
                if(tokens.size()!=3){usageError = true; break;}
                execute_rename(tokens[1], tokens[2]);
                break;
            case command::find:
                if(tokens.size()!=2){usageError = true; break;}
                execute_find(tokens[1]);
                break;
            case command::impfile:
                if(tokens.size()!=2){usageError = true; break;}
                execute_impfile(tokens[1]);
                break;
            case command::expfile:
                if(tokens.size()!=2){usageError = true; break;}
                execute_expfile(tokens[1]);
                break;
            case command::cd:
                if(tokens.size()!=2){usageError = true; break;}
                execute_cd(tokens[1]);
                break;
            case command::tree:
                if(tokens.size()!=1){usageError = true; break;}
                execute_printTree();
                std::cout<<std::endl;
                break;
            case command::quit:
                if(tokens.size()!=1){usageError = true; break;}
                std::cout<<"Exiting..."<<std::endl;
                return;
            case command::err:
                std::cout<<"Unrecognized command \'"<<tokens[0]<<"\'"<<std::endl;
                break;
        }
        if(usageError){
            std::cout<<std::endl<<"Improper usage of `"<<tokens[0]<<"`"<<std::endl;
            execute_help(tokens[0]);
        }
    } 
}
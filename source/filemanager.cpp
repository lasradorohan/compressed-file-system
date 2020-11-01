#include "filemanager.h"
#include "lz77.h"
#include <iostream>

std::vector<std::string> tokenize(const std::string str, const std::regex re) { 
    std::vector<std::string> tokenized(std::sregex_token_iterator(str.begin(), str.end(), re, -1), {}); 
    tokenized.erase(std::remove_if(tokenized.begin(),tokenized.end(), [](std::string const& s) { return s.size() == 0; }), tokenized.end());
    return tokenized; 
}

std::ifstream::pos_type filesize(std::string filename){
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}

File::File(std::string fname, Folder* parentFolder){
    this->entityName = fname;
    this->entityType = file;
    this->parentFolder = parentFolder;
    this->_fileindex = _filenamecount++;
    this->fileSize = 0;
    this->fileDiskSize = 0;
}

std::string File::_sizebytes(int nbytes){
    std::ostringstream temp;
    temp<<std::fixed<<std::setprecision(2);
    if(nbytes>std::pow(1024, 3)) temp<<nbytes/std::pow(1024, 3)<<" GB";
    else if(nbytes>std::pow(1024, 2)) temp<<nbytes/std::pow(1024, 2)<<" MB";
    else if(nbytes>1024) temp<<nbytes/1024<<" KB";
    else temp<<nbytes<<" B";
    return temp.str();
}
std::string File::_ratio(int a, int b){
    if(b==0) return "";
    std::ostringstream temp;
    temp<<std::fixed<<std::setprecision(2);
    temp<<(float)a/b*100<<" %";
    return temp.str();
}

std::string File::propertyString(){
    std::ostringstream temp;
    temp<<entityName<<" (file) "<<_sizebytes(fileDiskSize)<<" ("<<_sizebytes(fileSize)<<")";
    return temp.str();
}
std::vector<std::string> File::properties(){
    return std::vector<std::string>{entityName, "(file)", _sizebytes(fileSize), _sizebytes(fileDiskSize), _ratio(fileDiskSize, fileSize)};
}

void File::writeData(std::string content){
    std::string fname = "./bin/comp"+std::to_string(_fileindex)+".bin";
    std::fstream out(fname, std::fstream::out|std::fstream::binary);
    lz77::compress_t compress;
    out<<compress.feed(content);
    out.close();
    fileSize = content.size();
    fileDiskSize = filesize(fname);
}
std::string File::readData(){
    std::fstream in("./bin/comp"+std::to_string(_fileindex)+".bin", std::fstream::in);
    std::string compressed((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    lz77::decompress_t decompress;
    std::string extra;
    bool done = decompress.feed(compressed, extra);
    while (!done) done = decompress.feed(compressed, extra);
    return decompress.result();
}
int File::_filenamecount = 0;


Folder::Folder(std::string fname, Folder* parentFolder = nullptr){
    this->entityName = fname;
    this->entityType = folder;
    this->parentFolder = parentFolder;
}

std::vector<std::string> Folder::listContents(){
    std::vector<std::string> temp;
    if(contents.size()==0) return temp;
    std::transform(contents.begin(), contents.end(), std::back_inserter(temp), [](Entity* e){ return e->entityName; });
    return temp;
}
std::vector<std::string> Folder::listContentsFormatted(){
    std::vector<std::string> temp;
    temp.push_back("\033[3;104;30m.\033[0m");
    if(parentFolder!=nullptr) temp.push_back("\033[3;104;30m..\033[0m");
    if(contents.size()==0) return temp;
    std::transform(contents.begin(), contents.end(), std::back_inserter(temp), 
        [](Entity* e){
            if(e->entityType==file) return e->entityName;
            else return "\033[3;104;30m"+e->entityName+"\033[0m";
        }
    );
    return temp;
}

std::string Folder::propertyString(){
    std::ostringstream temp;
    temp<<entityName<<" (folder) ";
    return temp.str();
}
std::vector<std::string> Folder::properties(){
    return std::vector<std::string>{entityName, "(folder)", "", "", ""};
}

std::vector<std::string> Folder::listProps(){
    std::vector<std::string> temp;
    if(contents.size()==0) return temp;
    std::transform(contents.begin(), contents.end(), std::back_inserter(temp), [](Entity* e){ return e->propertyString(); });
    return temp;
}
std::vector<std::vector<std::string>> Folder::listPropsMat(){
    std::vector<std::vector<std::string>> temp;
    if(contents.size()==0) return temp;
    std::transform(contents.begin(), contents.end(), std::back_inserter(temp), [](Entity* e){ return e->properties(); });
    return temp;
}

int Folder::_length(){ return contents.size(); }

void Folder::addEntity(Entity* e){ contents.push_back(e); }

void Folder::removeEntity(int entityIndex){
    Entity* temp = contents[entityIndex];
    contents.erase(contents.begin()+entityIndex);
    delete temp;
}

FileManager::FileManager(){
    currentFolder = root;
    pathStrings.push_back(root->entityName);
}

FileManager::FileManager(Folder* folder): currentFolder(folder) {
    pathStrings.push_back(folder->entityName);
}

bool FileManager::_fileExists(Folder* folder, std::string fname){
    for(std::string temp: folder->listContents()) if(temp.compare(fname) == 0) return true;
    return false;
}
bool FileManager::_fileExists(Folder* folder, std::string fname, EntityType et){
    for(Entity* temp: folder->contents) if(temp->entityName.compare(fname) == 0 && temp->entityType == et) return true;
    return false;
}
bool FileManager::_fileExists(Folder* folder, std::regex re){
    for(const std::string& temp: folder->listContents()) if(std::regex_match(temp, re)) return true;
    return false;
}

int FileManager::_getIdx(Folder* currentFolder, std::string fname){
    int idx = 0;
    for(std::string temp: currentFolder->listContents()){
        if(temp.compare(fname) == 0) break;
        idx++;
    }
    return idx;
}

void FileManager::_recursiveDel(Entity* e){
    if(e->entityType ==  folder) {
        Folder* fol = dynamic_cast<Folder*>(e);
        if(fol->_length()==0){
            delete e;
        } else {
            for(Entity* temp: fol->contents){
                _recursiveDel(temp);
            }
        }
    } else if(e->entityType == file) {
        delete e;
    }
}

bool FileManager::_recursivefind(const std::regex& re, std::vector<std::string>& path, Folder* currfol){
    if(_fileExists(currfol, re)){
        path.push_back(currfol->entityName);
        return true;
    } else {
        for(Entity* e: currfol->contents){
            if(e->entityType==folder && _recursivefind(re, path, dynamic_cast<Folder*>(e))){
                path.push_back(currfol->entityName);
                return true;
            }
        }
        return false;
    }
}

FileManager::ErrorCode FileManager::_cd(Folder* fol){
    if(fol == nullptr){
        return FileManager::ErrorCode::FolderNotFound;
    } else {
        currentFolder = fol;
        return FileManager::ErrorCode::Pass;
    }
}

std::string FileManager::getCurrentPath(){
    std::ostringstream temp;
    std::copy(pathStrings.begin(), pathStrings.end(), std::ostream_iterator<std::string>(temp, os_delim));
    return temp.str();
}

void FileManager::printPromptString(std::ostream& out){
    out<<" \033[1;32m";
    std::copy(pathStrings.begin(), pathStrings.end(), std::ostream_iterator<std::string>(out, os_delim));
    out<<"\033[1;34m>\033[0m ";
}


std::vector<std::string> FileManager::listFiles(){ return currentFolder->listContentsFormatted(); }
std::vector<std::string> FileManager::listProps(){ return currentFolder->listProps(); }
std::vector<std::vector<std::string>> FileManager::listPropsMat(){ return currentFolder->listPropsMat(); }

FileManager::ErrorCode FileManager::importfile(std::string fname){
    if(_fileExists(currentFolder, fname)){
        return FileManager::ErrorCode::FileFolderExists;
    } else {
        File* newFile = new File(fname, currentFolder);
        std::fstream in("./import/"+fname, std::fstream::in);
        std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        newFile->writeData(content);
        currentFolder->contents.push_back(newFile);
        return FileManager::ErrorCode::Pass;
    }
}
FileManager::ErrorCode FileManager::exportfile(std::string fname){
    if(!_fileExists(currentFolder, fname)){
        return FileManager::ErrorCode::FileNotFound;
    } else {
        int idx = _getIdx(currentFolder, fname);
        File* thisFile = dynamic_cast<File*>(currentFolder->contents[idx]);
        std::string content = thisFile->readData();
        std::fstream out("./export/"+thisFile->entityName, std::fstream::out);
        out<<content;
        out.close();
        return FileManager::ErrorCode::Pass;
    }
}

FileManager::ErrorCode FileManager::makefile(std::string fname){
    if(_fileExists(currentFolder, fname)){
        return FileManager::ErrorCode::FileFolderExists;
    } else {
        currentFolder->contents.push_back(new File(fname, currentFolder));
        return FileManager::ErrorCode::Pass;
    }
}
FileManager::ErrorCode FileManager::writeFile(std::string fname, std::string content, bool noCheck = false){
    if(!noCheck && !_fileExists(currentFolder, fname)){
        return FileManager::ErrorCode::FileNotFound;
    } else {
        int idx = _getIdx(currentFolder, fname);
        File* thisFile = dynamic_cast<File*>(currentFolder->contents[idx]);
        thisFile->writeData(content);
        return FileManager::ErrorCode::Pass;
    }
}
FileManager::ErrorCode FileManager::readFile(std::string fname, std::string& buffer){
    if(!_fileExists(currentFolder, fname)){
        return FileManager::ErrorCode::FileNotFound;
    } else {
        int idx = _getIdx(currentFolder, fname);
        if(currentFolder->contents[idx]->entityType==folder) return FileManager::ErrorCode::IsFolder;
        File* thisFile = dynamic_cast<File*>(currentFolder->contents[idx]);
        buffer = thisFile->readData();
        return FileManager::ErrorCode::Pass;
    }
}
FileManager::ErrorCode FileManager::makefolder(std::string fname){
    if(_fileExists(currentFolder, fname)){
        return FileManager::ErrorCode::FileFolderExists;
    } else {
        currentFolder->contents.push_back(new Folder(fname, currentFolder));
        return FileManager::ErrorCode::Pass;
    }
}

FileManager::ErrorCode FileManager::rename(std::string fname, std::string newfname){
    if(!_fileExists(currentFolder, fname)) return FileManager::ErrorCode::FileNotFound;
    if(_fileExists(currentFolder, newfname)) return FileManager::ErrorCode::FileFolderExists;
    int idx = _getIdx(currentFolder, fname);
    currentFolder->contents[idx]->entityName = newfname;
    return FileManager::ErrorCode::Pass;
}

FileManager::ErrorCode FileManager::find(std::string rgxstr, std::string& buffer){
    std::vector<std::string> temppath;
    std::stringstream foundpath;
    bool found = _recursivefind(std::regex(rgxstr), temppath, currentFolder);
    if(!found){
        return FileManager::ErrorCode::FileFolderNotFound;
    } else {
        temppath.pop_back();
        std::copy(temppath.rbegin(), temppath.rend(), std::ostream_iterator<std::string>(foundpath, os_delim));
        buffer+=foundpath.str();
        return FileManager::ErrorCode::Pass;
    }
}

FileManager::ErrorCode FileManager::del(std::string fname){
    if(!_fileExists(currentFolder, fname)){
        return FileManager::ErrorCode::FileFolderNotFound;
    } else {
        int idx = _getIdx(currentFolder, fname);
        _recursiveDel(currentFolder->contents[idx]);
        currentFolder->contents.erase(currentFolder->contents.begin()+idx);
        return FileManager::ErrorCode::Pass;
    }
}

FileManager::ErrorCode FileManager::changeDir(std::string path){
    FileManager::ErrorCode flag = FileManager::ErrorCode::Pass;
    for(std::string fname: tokenize(path, std::regex("/"))){
        if(fname.compare("~")==0){
            pathStrings.clear();
            pathStrings.push_back(root->entityName);
            flag = _cd(root);
        } else if(fname.compare(".")==0) {
        } else if(fname.compare("..")==0) {
            if(currentFolder == root){
                flag = FileManager::ErrorCode::FolderNotFound;
            } else {
                pathStrings.pop_back();
                flag = _cd(currentFolder->parentFolder);
            }
        } else if(_fileExists(currentFolder, fname)) {
            int idx = _getIdx(currentFolder, fname);
            if(currentFolder->contents[idx]->entityType == file){
                flag = FileManager::ErrorCode::IsFile;
            } else {
                flag = _cd(dynamic_cast<Folder*>(currentFolder->contents[idx]));
                pathStrings.push_back(currentFolder->entityName);
            }
        } else {
            flag = FileManager::ErrorCode::FolderNotFound;
        }
        if(flag != FileManager::ErrorCode::Pass) return flag;
    }
    return flag;
}

void FileManager::_treerecursive(std::ostream& out, Folder* fol, int depth = 0){
    std::stringstream indentss;
    for(int i=0; i<depth; i++) indentss<<"│ ";
    std::string indent = indentss.str();
    for(Entity* e: fol->contents){
        out<<indent<<"├ "<<e->entityName<<std::endl;
        if(e->entityType==folder) _treerecursive(out, dynamic_cast<Folder*>(e), depth+1);
    }
}
void FileManager::printTree(std::ostream& out){
    std::cout<<currentFolder->entityName<<std::endl;
    _treerecursive(out, currentFolder);
}

Folder* FileManager::root  = new Folder(std::string("~"));
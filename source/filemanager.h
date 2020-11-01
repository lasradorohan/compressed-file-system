#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <fstream>
#include <iomanip>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iterator>
#include <regex>

class Folder;

enum EntityType{file, folder, none};

class Entity{
    public:
    std::string entityName;
    Folder* parentFolder;
    EntityType entityType;
    virtual std::string propertyString() = 0;
    virtual std::vector<std::string> properties() = 0;
    virtual ~Entity(){};
};

class File: public Entity{
    public:
    static int _filenamecount;
    File(std::string fname, Folder* parentFolder);
    int fileSize;
    int fileDiskSize;
    int _fileindex;

    std::string _sizebytes(int nbytes);
    std::string _ratio(int a, int b);
    std::string propertyString();
    std::vector<std::string> properties();
    void writeData(std::string content);
    std::string readData();
};

class Folder: public Entity{
    public:
    std::vector<Entity*> contents;
    
    Folder(std::string fname, Folder* parentFolder);
    
    std::vector<std::string> listContents();
    std::vector<std::string> listContentsFormatted();
    std::string propertyString();
    std::vector<std::string> properties();
    std::vector<std::string> listProps();
    std::vector<std::vector<std::string>> listPropsMat();
    int _length();
    void addEntity(Entity* e);
    void removeEntity(int entityIndex);
};

class FileManager{
    public:
    enum ErrorCode {Pass, FileNotFound, FolderNotFound, FileFolderNotFound, FileFolderExists, IsFile, IsFolder};
    const char* os_delim = "/";
    static Folder* root;
    Folder* currentFolder;
    std::vector<std::string> pathStrings;

    FileManager();
    FileManager(Folder* folder);

    bool _fileExists(Folder* folder, std::string fname);
    bool _fileExists(Folder* folder, std::string fname, EntityType et);
    bool _fileExists(Folder* folder, std::regex re);
    int _getIdx(Folder* currentFolder, std::string fname);
    void _recursiveDel(Entity* e);
    bool _recursivefind(const std::regex& re, std::vector<std::string>& path, Folder* currfol);
    ErrorCode _cd(Folder* fol);
    void _treerecursive(std::ostream& out, Folder* fol, int depth);
    std::string getCurrentPath();
    void printPromptString(std::ostream& out);
    std::vector<std::string> listFiles();
    std::vector<std::string> listProps();
    std::vector<std::vector<std::string>> listPropsMat();
    ErrorCode importfile(std::string fname);
    ErrorCode exportfile(std::string fname);
    ErrorCode makefile(std::string fname);
    ErrorCode writeFile(std::string fname, std::string content, bool noCheck);
    ErrorCode readFile(std::string fname, std::string& buffer);
    ErrorCode makefolder(std::string fname);
    ErrorCode rename(std::string fname, std::string newfname);
    ErrorCode find(std::string rgxstr, std::string& buffer);
    ErrorCode del(std::string fname);
    ErrorCode changeDir(std::string path);
    void printTree(std::ostream& out);
};

#endif
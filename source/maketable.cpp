#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

void maketable(std::ostream& out, std::vector<std::string> headers, std::vector<std::vector<std::string>> data, std::vector<int> rowwidths){
    int h = data.size(), w = data[0].size();
    out<<"┌";
    for(int i=0; i<w; i++){
        for(int j=0; j<rowwidths[i]; j++) out<<"─";
        if(i!=w-1)out<<"┬";
        else out<<"┐"<<std::endl;
    }
    out<<"│";
    for(int i=0; i<w; i++){
        out<<std::left<<std::setw(rowwidths[i])<<" "+headers[i];
        out<<"│";
    }
    out<<std::endl;
    out<<"├";
    for(int i=0; i<w; i++){
        for(int j=0; j<rowwidths[i]; j++) out<<"─";
        if(i!=w-1)out<<"┼";
        else out<<"┤"<<std::endl;
    }
    for(int i=0; i<h; i++){
        out<<"│";
        for(int j=0; j<w; j++){
            out<<std::left<<std::setw(rowwidths[j])<<" "+data[i][j];
            out<<"│";
        }
        out<<std::endl;
    }
    out<<"└";
    for(int i=0; i<w; i++){
        for(int j=0; j<rowwidths[i]; j++) out<<"─";
        if(i!=w-1)out<<"┴";
        else out<<"┘"<<std::endl;
    }
}

// void makelist(std::ostream& out, std::vector<std::string> data, int width){
//     out<<"┌";
//     for(int i=0; i<width; i++) out<<"─";
//     out<<"┐"<<std::endl;
//     for(int i=0; i<data.size(); i++){
//         out<<"│";
//         out<<std::left<<std::setw(width)<<" "+data[i];
//         out<<"│";
//         out<<std::endl;
//     }
//     out<<"└";
//     for(int i=0; i<width; i++) out<<"─";
//     out<<"┘"<<std::endl;
// }
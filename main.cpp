#include <iostream>
#include <vector>
#include "fstream"
#include "map"


struct IES{
    int vertical_num;
    int horizontal_num;

    std::vector<double> vertical_angle;
    std::vector<double> horizontal_angle;

    std::vector<double> cd;
};


//  def .split(" ") or .split("  ")
void split(std::string &line, std::vector<double> &box){
    int i, j = 0;
    std::string buff;

    for (const char c: line){
        if (c == ' '){
            if (i == 1) {
                box.push_back(atof(buff.c_str()));
                buff.clear();
                i = 0;
                j += 1;
            }
        } else{
            i = 1;
            buff += c;
        }
    }

    box.push_back(atof(buff.c_str()));
}


//  put all data in stock
void data_stock(std::string &line, std::vector<double> &stock){
    std::vector<double> box;
    split(line, box);

    for (double d: box){
        stock.push_back(d);
    }
}


//  set data
void config(IES *ies, std::vector<double> stock){

    ies->vertical_num = stock.at(3);
    ies->horizontal_num = stock.at(4);

    //  pass

    //  IES::vertical_angle
    std::copy(stock.begin()+13, stock.begin()+(ies->vertical_num + 13), std::back_inserter(ies->vertical_angle));

    //  IES::horizontal_angle
    std::copy(stock.begin()+(ies->vertical_num + 13), stock.begin()+(ies->vertical_num + 13 + ies->horizontal_num), std::back_inserter(ies->horizontal_angle));

    //  IES::cd
    std::copy(stock.begin()+(ies->vertical_num + 13 + ies->horizontal_num), stock.end(), std::back_inserter(ies->cd));

}


int main() {
    std::ifstream ifs("../LowBeam.ies");
    std::string line;
    std::vector<double> stock;
    IES ies;

    if (ifs.fail()){
        std::cerr << "File Open Error";
        return -1;
    }

    int i, cnt;


    //  read line by line
    while (std::getline(ifs, line)){

        //  find TILT line
        if (line.substr(0, 4) == "TILT"){
            i = 1;
            continue;
        }

        if (i == 1){
            data_stock(line, stock);
        }
    }

    config(&ies, stock);

    //  key: vertical_angle, horizontal_angle   /   value: cd
    std::map<std::pair<double, double>, double> vh_cd_map;
    for (int k = 0; k < ies.vertical_num; ++k) {
        for (int l = 0; l < ies.horizontal_num; ++l) {
            int idx = ies.vertical_num * l + k;               //  2d to 1d
            vh_cd_map[std::make_pair(ies.vertical_angle.at(k), ies.horizontal_angle.at(l))] = ies.cd.at(idx);
        }
    }


    //  tmp

    double v, h;
    std::cout << "vertical_angle: ";
    std::cin >> v;
    std::cout << "horizontal_angle: ";
    std::cin >> h;

    std::cout << vh_cd_map[std::make_pair(v, h)];

    //  end


    return 0;
}

#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <algorithm>
#define VERTICAL_NUM_INDEX 3
#define HORIZONTAL_NUM_INDEX 4
#define DATA_START_INDEX 13


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


void set_data(IES *ies, std::vector<double> stock){

    ies->vertical_num = stock.at(VERTICAL_NUM_INDEX);
    ies->horizontal_num = stock.at(HORIZONTAL_NUM_INDEX);

    //  pass

    //  IES::vertical_angle
    std::copy(stock.begin()+DATA_START_INDEX, stock.begin()+(ies->vertical_num + DATA_START_INDEX), std::back_inserter(ies->vertical_angle));

    //  IES::horizontal_angle
    std::copy(stock.begin()+(ies->vertical_num + DATA_START_INDEX), stock.begin()+(ies->vertical_num + DATA_START_INDEX + ies->horizontal_num), std::back_inserter(ies->horizontal_angle));

    //  IES::cd
    std::copy(stock.begin()+(ies->vertical_num + DATA_START_INDEX + ies->horizontal_num), stock.end(), std::back_inserter(ies->cd));

}


int get_adjacent(const std::vector<double> &vec, double d){
    for (int i = 0; i < vec.size()-1; ++i) {
        if ((vec.at(i) - d) * (vec.at(i+1) - d) < 0){
            return i;
        }
    }
    return -1;
}


double interpolation(IES &ies, std::map<std::pair<double, double>, double> &vh_cd_map, double v, double h, int mode){
    int v_adjacent_idx = get_adjacent(ies.vertical_angle, v);
    int h_adjacent_idx = get_adjacent(ies.horizontal_angle, h);

    //  h in ies.horizontal_angle
    if (mode == 0){
        if (v_adjacent_idx == -1){
            std::cerr << "Out of range";
            std::exit(-1);
        }

        double v0 = vh_cd_map[std::make_pair(ies.vertical_angle.at(v_adjacent_idx), h)];
        double v1 = vh_cd_map[std::make_pair(ies.vertical_angle.at(v_adjacent_idx + 1), h)];

        return (v1 - v0) / (ies.vertical_angle.at(v_adjacent_idx + 1) - ies.vertical_angle.at(v_adjacent_idx)) * (v - ies.vertical_angle.at(v_adjacent_idx)) + v0;
    }

    //  v in ies.vertical_angle
    else if (mode == 1){
        if (h_adjacent_idx == -1){
            std::cerr << "Out of range";
            std::exit(-1);
        }

        double h0 = vh_cd_map[std::make_pair(v, ies.horizontal_angle.at(h_adjacent_idx))];
        double h1 = vh_cd_map[std::make_pair(v, ies.horizontal_angle.at(h_adjacent_idx + 1))];

        return (h1 - h0) / (ies.horizontal_angle.at(h_adjacent_idx + 1) - ies.horizontal_angle.at(h_adjacent_idx)) * (h - ies.horizontal_angle.at(h_adjacent_idx)) + h0;
    }

    //  bi linear interpolation
    else{
        if (v_adjacent_idx == -1 || h_adjacent_idx == -1){
            std::cerr << "Out of range";
            std::exit(-1);
        }

        double v0_h0 = vh_cd_map[std::make_pair(ies.vertical_angle.at(v_adjacent_idx), ies.horizontal_angle.at(h_adjacent_idx))];
        double v1_h0 = vh_cd_map[std::make_pair(ies.vertical_angle.at(v_adjacent_idx + 1), ies.horizontal_angle.at(h_adjacent_idx))];
        double v0_h1 = vh_cd_map[std::make_pair(ies.vertical_angle.at(v_adjacent_idx), ies.horizontal_angle.at(h_adjacent_idx + 1))];
        double v1_h1 = vh_cd_map[std::make_pair(ies.vertical_angle.at(v_adjacent_idx + 1), ies.horizontal_angle.at(h_adjacent_idx + 1))];

        double v0_h0_to_v1_h0 = v0_h0 + (v1_h0 - v0_h0) * (v - ies.vertical_angle.at(v_adjacent_idx)) / (ies.vertical_angle.at(v_adjacent_idx + 1) - ies.vertical_angle.at(v_adjacent_idx));
        double v0_h1_to_v1_h1 = v0_h1 + (v1_h1 - v0_h1) * (v - ies.vertical_angle.at(v_adjacent_idx)) / (ies.vertical_angle.at(v_adjacent_idx + 1) - ies.vertical_angle.at(v_adjacent_idx));

        return v0_h0_to_v1_h0 + (v0_h1_to_v1_h1 - v0_h0_to_v1_h0) * (h - ies.horizontal_angle.at(h_adjacent_idx)) / (ies.horizontal_angle.at(h_adjacent_idx + 1) - ies.horizontal_angle.at(h_adjacent_idx));
    }
}


int main() {
    std::ifstream ifs("test.txt");
    std::string line;
    std::vector<double> stock;
    IES ies;

    if (ifs.fail()){
        std::cerr << "File Open Error";
        return -1;
    }

<<<<<<< HEAD
    int i;
=======
    bool found;
>>>>>>> bilinear

    //  read line by line
    while (std::getline(ifs, line)){

        //  find TILT line
        if (line.substr(0, 4) == "TILT"){
            found = true;
            continue;
        }

        if (found){
            data_stock(line, stock);
        }
    }

    set_data(&ies, stock);

    //  key: vertical_angle, horizontal_angle   /   value: cd
    std::map<std::pair<double, double>, double> vh_cd_map;
    for (int k = 0; k < ies.vertical_num; ++k) {
        for (int l = 0; l < ies.horizontal_num; ++l) {
            int idx = ies.vertical_num * l + k;               //  2d to 1d
            vh_cd_map[std::make_pair(ies.vertical_angle.at(k), ies.horizontal_angle.at(l))] = ies.cd.at(idx);
        }
    }

    /*
    double v, h;
    std::cout << "vertical_angle: ";
    std::cin >> v;
    std::cout << "horizontal_angle: ";
    std::cin >> h;
    */

    //test
    for (int j = 0; j <= 10; ++j) {
        for (int k = 0; k <= 20; ++k) {
            double v = k / 10.;
            double h = j / 10.;

            std::cout << "v : " << v << "\th : " << h << "\tcd : ";

            //  exe
            if (vh_cd_map.count(std::make_pair(v, h)) != 0) {
                std::cout << vh_cd_map[std::make_pair(v, h)];
            } else if (ies.vertical_angle.end() != std::find(ies.vertical_angle.begin(), ies.vertical_angle.end(), v)) {
                std::cout << interpolation(ies, vh_cd_map, v, h, 1);
            } else if (ies.horizontal_angle.end() !=
                       std::find(ies.horizontal_angle.begin(), ies.horizontal_angle.end(), h)) {
                std::cout << interpolation(ies, vh_cd_map, v, h, 0);
            } else {
                std::cout << interpolation(ies, vh_cd_map, v, h, 2);
            }
            //  end

            std::cout << std::endl;
        }
    }

    
    return 0;
}
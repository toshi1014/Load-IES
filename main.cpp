#include <iostream>
#include <vector>
#include "fstream"
#include "map"
#include "algorithm"


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


int get_adjacent(const std::vector<double> &vec, double d){
    for (int i = 0; i < vec.size()-1; ++i) {
        if ((vec.at(i) - d) * (vec.at(i+1) - d) < 0){
            return i;
        }
    }
    return -1;
}


double interpolation(IES &ies, std::map<std::pair<double, double>, double> &vh_cd_map, double v, double h, int mode){
    int v_adjacent = get_adjacent(ies.vertical_angle, v);
    int h_adjacent = get_adjacent(ies.horizontal_angle, h);

    //  h in ies.horizontal_angle
    if (mode == 0){
        if (v_adjacent == -1){
            std::cerr << "Out of range";
            std::exit(-1);
        }

        double fst = vh_cd_map[std::make_pair(ies.vertical_angle.at(v_adjacent), h)];
        double snd = vh_cd_map[std::make_pair(ies.vertical_angle.at(v_adjacent + 1), h)];

        return (snd - fst) / (ies.vertical_angle.at(v_adjacent+1) - ies.vertical_angle.at(v_adjacent)) * (v - ies.vertical_angle.at(v_adjacent)) + fst;
    }

    //  v in ies.vertical_angle
    else if (mode == 1){
        if (h_adjacent == -1){
            std::cerr << "Out of range";
            std::exit(-1);
        }

        double fst = vh_cd_map[std::make_pair(v, ies.horizontal_angle.at(h_adjacent))];
        double snd = vh_cd_map[std::make_pair(v, ies.horizontal_angle.at(h_adjacent + 1))];

        return (snd - fst) / (ies.horizontal_angle.at(h_adjacent+1) - ies.horizontal_angle.at(h_adjacent)) * (h - ies.horizontal_angle.at(h_adjacent)) + fst;
    }
    else {
        if (v_adjacent == -1 || h_adjacent == -1){
            std::cerr << "Out of range";
            std::exit(-1);
        }

        double fst = vh_cd_map[std::make_pair(ies.vertical_angle.at(v_adjacent), ies.horizontal_angle.at(h_adjacent))];
        double snd = vh_cd_map[std::make_pair(ies.vertical_angle.at(v_adjacent + 1), ies.horizontal_angle.at(h_adjacent))];
        double thd = vh_cd_map[std::make_pair(ies.vertical_angle.at(v_adjacent), ies.horizontal_angle.at(h_adjacent + 1))];
        double fth = vh_cd_map[std::make_pair(ies.vertical_angle.at(v_adjacent), ies.horizontal_angle.at(h_adjacent + 1))];

        double fst_to_snd[3] = {
                ies.vertical_angle.at(v_adjacent + 1) - ies.vertical_angle.at(v_adjacent),
                0.,
                snd - fst
        };

        double fst_to_thd[3] = {
                0.,
                ies.horizontal_angle.at(h_adjacent + 1) - ies.horizontal_angle.at(h_adjacent),
                thd - fst
        };


        double rel_v = v - ies.vertical_angle.at(v_adjacent);
        double rel_h = h - ies.horizontal_angle.at(h_adjacent);

        if (rel_v / fst_to_snd[0] < 0.5 || rel_h / fst_to_thd[1] < 0.5){
            double fth_to_snd[3] = {
                    0.,
                    ies.horizontal_angle.at(h_adjacent + 1) - ies.horizontal_angle.at(h_adjacent),
                    fth - snd
            };
            double fth_to_thd[3] = {
                    ies.vertical_angle.at(v_adjacent + 1) - ies.vertical_angle.at(v_adjacent),
                    0.,
                    fth - thd
            };
            std::cout << "fth : ";

            return -fth_to_snd[2] * rel_v / fth_to_snd[1] - fth_to_thd[2] * rel_h / fth_to_thd[1] + fth;

        }

        return fst_to_snd[2] * rel_v / fst_to_snd[0] + fst_to_thd[2] * rel_h / fst_to_thd[1] + fst;
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

    int i;

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


    double v, h;
    std::cout << "vertical_angle: ";
    std::cin >> v;
    std::cout << "horizontal_angle: ";
    std::cin >> h;

    if (vh_cd_map.count(std::make_pair(v, h)) != 0){
        std::cout << vh_cd_map[std::make_pair(v, h)];
    } else if (ies.vertical_angle.end() != std::find(ies.vertical_angle.begin(), ies.vertical_angle.end(), v)){
        std::cout << interpolation(ies, vh_cd_map, v, h, 1);
    } else if (ies.horizontal_angle.end() != std::find(ies.horizontal_angle.begin(), ies.horizontal_angle.end(), h)){
        std::cout << interpolation(ies, vh_cd_map, v, h, 0);
    } else {
        std::cout << interpolation(ies, vh_cd_map, v, h, 2);
    }

    return 0;
}
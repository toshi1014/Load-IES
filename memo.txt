#include <iostream>
#include <vector>
#include <algorithm>
#include "fstream"
#include "map"


struct IES{
    int vertical_num;
    int horizontal_num;

    std::vector<double> vertical_angle;
    std::vector<double> horizontal_angle;

    std::vector<double> cd;
};


void get_line_length(const int *len, int *line_length){
    double x = *len / 10.0;
    int y = static_cast<int>(x);
    if (x-y == 0){
        *line_length = y;
    } else{
        *line_length = y + 1;
    }
}


//  def .split(" ") or .split("  ")
void split(std::string &line, std::string box[]){
    int i, j = 0;
    std::string buff;

    for (const char c: line){
        if (c == ' '){
            if (i == 1) {
                box[j] = buff;
                buff.clear();
                i = 0;
                j += 1;
            }
        } else{
            i = 1;
            buff += c;
        }
    }

    box[j] = buff;
}


//  read first line
void get_first_line(IES *ies, std::string &line){
    int item = 10;
    std::string box[item];
    split(line, box);

    ies->vertical_num = atoi(box[3].c_str());
    ies->horizontal_num = atoi(box[4].c_str());

    //  pass

}


//  read second line
void get_second_line(){}


//  read vertical_angle || horizontal_angle || cd
void get_data(IES *ies, std::string &line, int i){
    std::string box[10];
    split(line, box);
    for (std::string s: box){
        if (!s.empty()){
            if (i == 0)
                ies->vertical_angle.push_back(atof(s.c_str()));
            else if (i == 1)
                ies->horizontal_angle.push_back(atof(s.c_str()));
            else
                ies->cd.push_back(atof(s.c_str()));
        }
    }
}



int main() {
    std::ifstream ifs("../LowBeam.ies");
    std::string line;
    IES ies;

    if (ifs.fail()){
        std::cerr << "File Open Error";
        return -1;
    }

    int i = 0;
    int j = 1;                          //  line marker
    int vertical_line_length;
    int horizontal_line_length;

    //  read line by line
    while (std::getline(ifs, line)){

        //  find TILT line
        if (line.substr(0, 4) == "TILT"){
            i = 1;
            continue;
        }

        //  get data on first line
        if (i == 1){
            get_first_line(&ies, line);
            get_line_length(&ies.vertical_num, &vertical_line_length);
            get_line_length(&ies.horizontal_num, &horizontal_line_length);

            i = 2;
            continue;
        }

        //  get data on second line
        if (i == 2){
            get_second_line();

            i = 3;
            continue;
        }


        //  get vertical_angle
        if (i == 3){
            get_data(&ies, line, 0);
            if (j == vertical_line_length){
                i = 4;
                j = 1;
                continue;
            } else{
                j += 1;
            }
        }

        //  get horizontal_angle
        if (i == 4){
            get_data(&ies, line, 1);
            if (j == horizontal_line_length){
                i = 5;
                continue;
            } else{
                j += 1;
            }
        }

        //  get cd
        if (i == 5){
            get_data(&ies, line, 2);
        }

    }
    //  read end



    //  key: vertical_angle, horizontal_angle   /   value: cd
    std::map<std::pair<double, double>, double> vh_cd_map;
    for (int k = 0; k < ies.vertical_num; ++k) {
        for (int l = 0; l < ies.horizontal_num; ++l) {
            int idx = ies.vertical_num * l + k;               //  2d to 1d
            vh_cd_map[std::make_pair(ies.vertical_angle.at(k), ies.horizontal_angle.at(l))] = ies.cd.at(idx);
        }
    }

    std::cout << vh_cd_map[std::make_pair(-15., 60.)];


    return 0;
}

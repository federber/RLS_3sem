#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include "rls_utils.h"
#include "rls_evaluator.h"
#include "points.h"

#include"reading_parser.h"

std::vector<std::vector<double> > readFileData(const std::string& filename) {
    std::vector<std::vector<double> > data;
    std::ifstream file(filename);

    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return data;
    }

    std::string line;
    std::getline(file, line);  

    while (std::getline(file, line)) {
        std::vector<double> row;
        std::istringstream iss(line);
        double value;

        while (iss >> value) {
            row.push_back(value);
        }

        data.push_back(row);
    }

    return data;
}


std::map<size_t, std::vector<GeoData> > parser_geodata(std::vector<std::vector<double> > matrix){
    std::map<size_t, std::vector<GeoData> > res;

    for(int row = 0; row < matrix.size(); ++row){
        size_t key = matrix[row][8];
        res[key].push_back({
                                matrix[row][5],
                                size_t(matrix[row][6]),
                                size_t(matrix[row][7]),
                                size_t(matrix[row][8]),
                                matrix[row][9],
                                matrix[row][10],
                                matrix[row][11],
                                matrix[row][12]
                            });
    }
    
    return res;
}


std::map<size_t, std::vector<Etalon_point> > parser_etalon(std::vector<std::vector<double> > matrix){
    std::map<size_t, std::vector<Etalon_point> > res;

    for(int row = 0; row < matrix.size(); ++row){
        size_t key = matrix[row][2];
        res[key].push_back({ size_t(matrix[row][2]),
                        matrix[row][5],
                        matrix[row][7],
                        matrix[row][8],
                        matrix[row][9],
                        { matrix[row][16], matrix[row][17], matrix[row][18] },
                        { matrix[row][19], matrix[row][20], matrix[row][21] },
                        { matrix[row][22], matrix[row][23], matrix[row][24] }
                    });
    }
    
    return res;
}

std::map<size_t, std::vector<Point3D> > parser_point(std::vector<std::vector<double> > matrix){
    std::map<size_t, std::vector<Point3D> > res;
    int column = 0; 
    while (column < matrix[0].size()){
        std::size_t key = 1;
        for(int row = 0; row < matrix.size(); ++row){
            res[key].push_back({ matrix[row][column], matrix[row][column+1], 0});
        }
        ++key;
        column = column + 2;
    }
    
    return res;
}


void Solution::SetData() 
     {coordconverter = Coord_converter();}

void Solution::GetFactors()
     {evaluator = Evaluator();}

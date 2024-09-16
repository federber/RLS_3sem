#ifndef READING_PARSER_H
#define READING_PARSER_H

#include "rls_utils.h"
#include "coordsystem.h"
#include "points.h"
#include "rls_evaluator.h"
#include <cstdlib>



std::vector<std::vector<double>> readFileData(const std::string& filename); //считывает данные из файла filename

std::map<std::size_t, std::vector<GeoData>> parser_geodata(std::vector<std::vector<double>> matrix); //преобразует данные из файла, который уже считали, в std::map<size_t, std::vector<GeoData>>


std::map<std::size_t, std::vector<Etalon_point>> parser_etalon(std::vector<std::vector<double>> matrix); //преобразует данные из файла, который уже считали, в std::map<size_t, std::vector<Etalon_point>>


std::map<std::size_t, std::vector<Point3D>> parser_point(std::vector<std::vector<double>> matrix); //преобразует данные из файла, который уже считали, в std::map<size_t, std::vector<Point3D>>


class Solution { //протокол обмена данными
private:
    Coord_converter coordconverter;
    Evaluator evaluator;

public:
    void SetData(); //получает эталонные данные
    void GetFactors(); //набор вычисленных значений
};

#endif // READING_PARSER_H

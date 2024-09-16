#ifndef COORD_CONV_TEST_H
#define COORD_CONV_TEST_H
#include<map>
#include<vector>
#include<iostream>

#include"rls_utils.h"
#include "coordsystem.h"

void test_conv_GeoC_geoD();
void test_conv_spherical();
void show(std::map<size_t, std::vector<Etalon_point>> inp, std::map<size_t, std::vector<GeoData>> res);

#endif // COORD_CONV_TEST_H

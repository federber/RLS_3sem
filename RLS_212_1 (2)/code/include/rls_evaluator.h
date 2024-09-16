#ifndef RLS_EVALUATOR_H
#define RLS_EVALUATOR_H
#include<map>
#include<iostream>
#include<vector>
#include<cmath>

#include"rls_utils.h"

struct Eval_param // структура описывающая показатели качества ПОИ
{
    //точностные характеристики в формате: полная ошибка, систем. ошибка, СКО
    Point3D dist_accur;     // точность измерения дальности, м
    Point3D azim_accur;     // точность измерения азимута, матем.градусы
    Point3D aim_ang_accur;   // точность измерения угла места, матем.градусы
    Point3D rad_sp_accur;    // точность  измерения радиальной скорости, м/с
    std::pair<double,double> min_dist;   // минимальная дальность обнаружения, при высоте полета цели м,м
    std::pair<double,double> max_dist;   // максимальная дальность обнаружения, при высоте полета цели м,м
    double rh_rlz_coef;  // коэффициент реализации радиогоризонта
};

struct Req_param
{
    double T_obz; // период обзора РЛС
    double L_sb; // минимальное кол-во отметок для сбора тарссы с сопровождения
    double h_c; // высота подъема центра антенны
    double P_lt = std::pow(10, -5); // вероятность ложной тревоги
    std::size_t N_obz = 5; // общее число обзоров РЛС на i-м интервале усреднения по дальности
};
std::ostream& operator<<(std::ostream &out, Req_param& rp);
std::ostream& operator<<(std::ostream &out, Eval_param& ep);


class Evaluator // класс для оценки показателей работы РЛС
{
    std::map<std::size_t, std::vector<GeoData>> poi_result;
    std::map<std::size_t, std::vector<GeoData>> traces;
    std::map<std::size_t, std::vector<GeoData>> interpolated_tr;
    std::map<std::size_t, Eval_param> ev_param; // показатели качества ПОИ

    void eval_dist_accur(std::size_t tr_num); // оценка точности измерения дальности
    void eval_azim_accur(std::size_t tr_num); // оценка точности измерения азимута
    void eval_aim_ang_accur(std::size_t tr_num); // оценка точности измерения угла места
    void eval_rad_sp_accur(std::size_t tr_num); // оценка точности измерения радиальной скорости
    void eval_rh_rlz_coef(std::size_t tr_num, const Req_param&); // оценка коэффициента реализации радиогоризонта
    void eval_minmax_dist(std::size_t tr_num, const Req_param&); // оценка минимальной и максимальной дальностей обнаружения

public:
    void setPoiRes(const std::map<std::size_t, std::vector<GeoData>>&);
    void setTraces(const std::map<std::size_t, std::vector<GeoData>>&);

    //void interpolate(); // провести интерполяцию для всех трасс
    void interpolate(std::size_t poi_id, std::size_t etalon_id); // провести интерполяцию эталонной трассы etalon_id к резутьтатом ПОИ с индексом poi_id
    std::map<std::size_t, std::vector<GeoData>> getInterpResult() const;

    Eval_param getEvalParam(std::size_t tr_num, const Req_param& par); // получить показатели качества ПОИ

};

double interp_coord(double t0, double t1, double x0, double x1, double t); // интерполяция для одного параметра
GeoData binsearch_interp(const std::vector<GeoData>&, const GeoData&); // интерполяция для одной КТ
std::size_t num_of_detect_on_segm(const std::vector<GeoData>&, double a, double b); // возвращает количество обнаружений цели на отрезке [a,b] по дальности


#endif // RLS_EVALUATOR_H

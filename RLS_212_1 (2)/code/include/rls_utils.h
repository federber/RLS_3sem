#ifndef RLS_UTILS
#define RLS_UTILS

#include<map>
#include<iostream>
#include<vector>
#include"points.h"
#include"coordsystem.h"

struct M_vec //описывает математический вектор в трехмерном пространстве
{
    double x;
    double y;
    double z;
    double mod() const;//возвращает модуль скорости
    Point3D getP3D() const;
};


struct Etalon_point // описывает одну координатную точку из эталонных данных
{
    std::size_t trace_id; // идентификатор трассы
   // std::size_t cp_id;
    double mod_time; // время, которому соответствует координатная точка
    double latitude; // широта
    double longitude; // долгота
    double height; // высота
    M_vec coord; // вектор координат в прямоугольной системе координат
    M_vec speed; // вектор скорости в ПСК
    M_vec accel; // вектор ускорения в ПСК

    bool operator<(const Etalon_point&) const;
    bool operator<=(const Etalon_point&) const;
    bool operator==(const Etalon_point&) const;

};


struct GeoData // описывает координатную точку в сферических координатах. Внутренний формат
{
    double mod_time; // время, которому соответствует координатная точка
    std::size_t zond_num = 1; // номер зондирования
    std::size_t cp_id; // идентификатор(номер) координатной точки
    std::size_t trace_id; // номер трассы
    double azim; // азимут
    double oblique_dist; // наклонная дальность
    double aim_angle; // угол места
    double rad_speed = 0; // радиальная скорость

    bool operator<(const GeoData&) const;
    bool operator<=(const GeoData&) const;
    bool operator==(const GeoData&) const;
    GeoData();
    GeoData(double,std::size_t,std::size_t,std::size_t,double,double,double,double);
};

std::ostream& operator<<(std::ostream &out, Etalon_point& ep);
std::ostream& operator<<(std::ostream &out, M_vec& mv);
std::ostream& operator<<(std::ostream &out, GeoData& ep);
class Coord_converter
{
    std::map<std::size_t, std::vector<Etalon_point>> traces; // трассы, где ключ - номер трассы, значение - вектор КТ
    M_vec rls_coord; // координаты стояния РЛС

    std::pair<std::string, std::string> inp_param_type = {"WGS84", "WGS84"}; // название начальной СК; first-трассы, second-координаты РЛС

    std::map<std::size_t, std::vector<GeoData>> converse_to_GeoData(const std::map<std::size_t, std::vector<Etalon_point>>&); // внутренняя функция для преобразования координат

public:
    Coord_converter(): rls_coord{55.89558, 41.206408, 177} {}; // конструктор по умолчанию
    void set_etalon_traces(const std::map<std::size_t, std::vector<Etalon_point>>&); // задать эталонные трассы
    void set_rls_coodr(double, double, double); // задать координаты РЛС
    void set_rls_coodr(const M_vec&); // задать координаты РЛС
    void set_inp_par_type(const std::pair<std::string, std::string>&); // задать название начальной СК

    std::map<std::size_t, std::vector<GeoData>> get_etalon_traces_spher(); // получить данные эталонных трасс в сферических координатах
    M_vec get_rls_coord(); // получить координаты РЛС
    std::pair<std::string, std::string> get_inp_par_type() const; // получить названия начальных СК


    std::map<std::size_t, std::vector<std::pair<double,double>>> conv_from_sph_to_tpc(std::map<std::size_t, std::vector<GeoData>>) const; // для графиков, проекция на XY. преобразование в топоцентрическую СК с началом в точке стояния РЛС
    std::map<std::size_t, std::vector<Point3D>> conv_from_tpc3_to_sph(std::map<std::size_t, std::vector<Point3D>>) const; // для таблиц, преобразование из токоцентрической СК в сферическую. предполагается, что начало везде в this->rls_coord

};

bool is_equal(double x, double y); // проверяет, находится ли расхождение абсолютных значений х и у в пределах малой окрестности, eps=0.0002
bool is_equal(const M_vec& v1, const M_vec& v2); // покоординатно вызывает is_equal(double,double)

#endif //RLS_UTILS

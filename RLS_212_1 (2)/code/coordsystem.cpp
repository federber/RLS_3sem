#include <iostream>

#include "coordsystem.h"

#define UTMScaleFactor 0.9996
#define R_ekv 8500000 // эквивалентный радиус Земли, метры

// Конструкторы -----------------------------------------------------------------------------------------------------------------------------------------
/// принимает большую, малую полуось и коэффициент сжатия в -1 степени
Ellipsoid::Ellipsoid(double maj, double min, double compar) :
    major_axis{maj}, minor_axis{min}, comprassion{1 / compar},
    e_1{std::sqrt(std::pow(maj, 2) - std::pow(min, 2)) / maj},
    e_2{std::sqrt(std::pow(maj, 2) - std::pow(min, 2)) / min}
{ }

/// принимает геоцентрические координаты, параметр трансформирования и матрицу трансформирования
CoordSystem::CoordSystem(const Point3D& coord, const std::vector<double>& ellipsoid_par) :
    ellipsoid{ellipsoid_par[0], ellipsoid_par[1], ellipsoid_par[2]},  geoD{coord}
{
    double a = ellipsoid.major_axis;
    double e_squared = std::pow(ellipsoid.e_1, 2); // первый эксцентреситет в квадрате
    double sin_B_squared = std::pow(std::sin(geoD.B()), 2); // sin(B) в квадрате
    merid_rad = a * (1 - e_squared) / std::sqrt(1 - e_squared * sin_B_squared);
    vertic_rad = a / std::sqrt(1 - e_squared * sin_B_squared);

    double cosB = std::cos(geoD.B());
    double cosL = std::cos(geoD.L());
    double sinB = std::sin(geoD.B());
    double sinL = std::sin(geoD.L());
    double N = vertic_rad;
    double x = (N + geoD.H()) * cosB * cosL;
    double y = (N + geoD.H()) * cosB * sinL;
    double z = (N + geoD.H() - e_squared * N) * sinB;

    geoC.setCoord(x, y, z);
}

SK42::SK42(const Point3D& coord) :
    CoordSystem{coord,
                std::vector<double>{sk42::a, sk42::b, sk42::f}}
{}

WGS84::WGS84(const Point3D& coord) :
    CoordSystem{coord,
                std::vector<double>{wgs84::a, wgs84::b, wgs84::f}}
{}

PZ90_11::PZ90_11(const Point3D& coord) :
    CoordSystem{coord,
                std::vector<double>{pz90_11::a, pz90_11::b, pz90_11::f}}
{}

SK42::SK42() :
    CoordSystem{Point3D{0, 0, 0},
                std::vector<double>{sk42::a, sk42::b, sk42::f}}
{}

WGS84::WGS84() :
    CoordSystem{Point3D{0, 0, 0},
                std::vector<double>{wgs84::a, wgs84::b, wgs84::f}}
{}

PZ90_11::PZ90_11() :
    CoordSystem{Point3D{0, 0, 0},
                std::vector<double>{pz90_11::a, pz90_11::b, pz90_11::f}}
{}

// Вывод данных ------------------------------------------------------------------------------------------------------------------------------------------
void Ellipsoid::print() const
{
    std::cout << "major_axis: " << major_axis << std::endl;
    std::cout << "minor_axis: " << minor_axis << std::endl;
    std::cout << "comprassion: " << comprassion << std::endl;
    std::cout << "first eccentricity: " << e_1 << std::endl;
    std::cout << "second eccentricity: " << e_2 << std::endl;
}

void CoordSystem:: print() const
{
    std::cout << "geoD: " << geoC << std::endl;
    std::cout << "geoC: " << geoD << std::endl;
    ellipsoid.print();
}


//  Получение приватных полей классов и присваивание новых координат ---------------------------------------------------------------
Point3D CoordSystem::getGeoD() const
{
    double h = geoD.H();
    Point3D coord{geoD};
    coord.setCoord(coord.B(), coord.L(), h);
    return coord;
}

Point3D CoordSystem::getGeoC() const
{ return geoC; }

Ellipsoid CoordSystem::getEllipsoid() const
{ return ellipsoid; }

void CoordSystem::setGeoD(const Point3D& coord)
{
    geoD = coord;
    double a = ellipsoid.major_axis;
    double e_squared = std::pow(ellipsoid.e_1, 2); // первый эксцентреситет в квадрате
    double sin_B_squared = std::pow(std::sin(geoD.B()), 2); // sin(B) в квадрате
    merid_rad = a * (1 - e_squared) / std::sqrt(1 - e_squared * sin_B_squared);
    vertic_rad = a / std::sqrt(1 - e_squared * sin_B_squared);

    double cosB = std::cos(geoD.B());
    double cosL = std::cos(geoD.L());
    double sinB = std::sin(geoD.B());
    double sinL = std::sin(geoD.L());
    double N = vertic_rad;
    double x = (N + geoD.H()) * cosB * cosL;
    double y = (N + geoD.H()) * cosB * sinL;
    double z = (N + geoD.H() - e_squared * N) * sinB;

    geoC.setCoord(x, y, z);
}

void CoordSystem::setGeoC(const Point3D& coord)
{
    geoC = coord;
    double e_squared = std::pow(ellipsoid.e_1, 2); // первый эксцентреситет в квадрате
    double epsilon = e_squared / (1 - e_squared);
    double b = ellipsoid.minor_axis; // малая полуось эллипсоида
    double a = ellipsoid.major_axis; // большая полуось эллипсоида
    double p = std::sqrt(geoC.X() * geoC.X() + geoC.Y() * geoC.Y());
    double Q = std::atan2(geoC.Z() * a, p * b);
    double sinQ_cubic = std::pow(std::sin(Q), 3); // sin(Q) в кубе
    double cosQ_cubic = std::pow(std::cos(Q), 3); // cos(Q) в кубе

    double B = std::atan2(geoC.Z() + epsilon * b * sinQ_cubic, p - e_squared * a * cosQ_cubic);
    double L = std::atan2(geoC.Y(),  geoC.X());
    double H = p / std::cos(B) - vertic_rad;
    geoD.setCoord(B, L, H);
}


// Конструкторы матриц перехода -----------------------------------------------------------------------------------------------------------------
Matrix make_transit_matrix(const Point3D& w) /// создаёт матрицу перехода
{ return Matrix{std::vector<double>{1, -w.Z(), w.Y(), w.Z(), 1, -w.X(), -w.Y(), w.X(), 1}, 3, 3}; }

Matrix make_reverse_transit_matrix(const Point3D& w) /// создаёт матрицу обратного перехода
{
    Matrix reverse_transit_mat = make_transit_matrix(w) * (-1);
    reverse_transit_mat[0][0] = 1.;
    reverse_transit_mat[1][1] = 1.;
    reverse_transit_mat[2][2] = 1.;
    return reverse_transit_mat;
}

// Переход  между разными СК ------------------------------------------------------------------------------------------------------------------------

/// в WGS 84
WGS84& WGS84::operator= (const SK42& sk)
{
    double par = wgs84::m_sk42 * std::pow(10, -6);
    Point3D delta = wgs84::delta_sk42;
    Matrix reverse_transit_mat = make_reverse_transit_matrix(wgs84::w_sk42);
    Point3D coord = (1 + par) * sk.getGeoC() * reverse_transit_mat + delta;
    setGeoC(coord);
    return *this;
}

WGS84& WGS84::operator =(const PZ90_11& pz)
{
    SK42 sk(Point3D(0, 0, 0));
    sk = pz;
    *this = sk;
    return *this;
}

WGS84& WGS84::operator= (const WGS84& wgs)
{
    this->setGeoC(wgs.geoC);
    this->setGeoD(wgs.geoD);
    return *this;
}
/// в СК 42
SK42& SK42::operator= (const WGS84& wgs)
{
    double par = wgs84::m_sk42 * std::pow(10, -6);
    Point3D delta = wgs84::delta_sk42;
    Matrix transit_mat = make_transit_matrix(wgs84::w_sk42);
    Point3D coord = (1 - par) * wgs.getGeoC() * transit_mat  - delta;
    setGeoC(coord);
    return *this;
}

SK42& SK42::operator= (const PZ90_11& pz)
{
    double par = sk42::m_pz90_11 * std::pow(10, -6);
    Point3D delta = sk42::delta_pz90_11;
    Matrix transit_mat = make_transit_matrix(sk42::w_pz90_11);
    Point3D coord = (1 - par) * pz.getGeoC() * transit_mat  + delta;
    setGeoC(coord);
    return *this;
}

SK42& SK42::operator= (const SK42& sk)
{
    this->setGeoC(sk.geoC);
    this->setGeoD(sk.geoD);
    return *this;
}

/// в ПЗ-90.11
PZ90_11& PZ90_11::operator= (const SK42& sk)
{
    double par = sk42::m_pz90_11 * std::pow(10, -6);
    Point3D delta = sk42::delta_pz90_11;
    Matrix reverse_transit_mat = make_reverse_transit_matrix(sk42::w_pz90_11);
    Point3D coord = (1 + par) * sk.getGeoC() * reverse_transit_mat + delta;
    setGeoC(coord);
    return *this;
}

PZ90_11& PZ90_11::operator= (const WGS84& wgs)
{
    SK42 sk(Point3D(0, 0, 0));
    sk = wgs;
    *this = sk;
    return *this;
}

PZ90_11& PZ90_11::operator= (const PZ90_11& pz)
{
    this->setGeoC(pz.geoC);
    this->setGeoD(pz.geoD);
    return *this;
}
/*
LRTCS::LRTCS(const WGS84& coord, const WGS84& rls_c){
    Point3D geoD = coord.getGeoD();
    Point3D rls_geoD = rls_c.getGeoD();
    Ellipsoid el_c = coord.getEllipsoid();
    Ellipsoid el_rls = rls_c.getEllipsoid();
    double sinB = sin(geoD.B());
    double sinB0 = sin(rls_geoD.B());
    double cosB = cos(geoD.B());
    double cosB0 = cos(rls_geoD.B());
    double rcv1_B = el_c.major_axis / sqrt(1 - pow(el_c.e_1 * sinB, 2));
    double rcv1_B0 = el_rls.major_axis / sqrt(1 - pow(el_rls.e_1 * sinB0, 2));
    double U = (rcv1_B + geoD.H()) * cosB * sin(geoD.L() - rls_geoD.L());
    double V = (rcv1_B + geoD.H()) * (sinB * cosB0 - cosB * sinB0 * cos(geoD.L() - rls_geoD.L()))
                + pow(el_c.e_1, 2) * (rcv1_B0 * sinB0 - rcv1_B * sinB) * cosB0;
    double W = (rcv1_B + geoD.H()) * (sinB * sinB0 + cosB * cosB0 * cos(geoD.L() - rls_geoD.L()))
                + pow(el_c.e_1, 2) * (rcv1_B0 * sinB0 - rcv1_B * sinB) * sinB0 - (rcv1_B0 + rls_geoD.H());
    double h = std::sqrt(std::pow(V,2) + std::pow(U,2) + std::pow(R_ekv+W,2))-R_ekv;
    lrtcs_coord.setCoord(U,V,h);
    rls_coord = rls_c;
}
*/

LRTCS::LRTCS(const WGS84& coord, const WGS84& rls_c)
{
    Point3D geoC = coord.getGeoC();
    Point3D rls_geoC = rls_c.getGeoC();
    Point3D rls_geoD = rls_c.getGeoD();
    double sinL0 = sin(rls_geoD.L());
    double sinB0 = sin(rls_geoD.B());
    double cosL0 = cos(rls_geoD.L());
    double cosB0 = cos(rls_geoD.B());
    double U = -(geoC.X() - rls_geoC.X()) * sinL0 + (geoC.Y() - rls_geoC.Y()) * cosL0;
    double V = -(geoC.X() - rls_geoC.X()) * sinB0 * cosL0 - (geoC.Y() - rls_geoC.Y()) * sinB0 * sinL0 + (geoC.Z() - rls_geoC.Z()) * cosB0;
    double W =  (geoC.X() - rls_geoC.X()) * cosB0 * cosL0 + (geoC.Y() - rls_geoC.Y()) * cosB0 * sinL0 + (geoC.Z() - rls_geoC.Z()) * sinB0;
    double h = std::sqrt(std::pow(V,2) + std::pow(U,2) + std::pow(R_ekv+W,2))-R_ekv;
    lrtcs_coord.setCoord(U,V,h);
    rls_coord = rls_c;
}

double LRTCS::get_H() const
{
    return sqrt(pow(lrtcs_coord.X(), 2) + pow(lrtcs_coord.Y(), 2) + pow(R_ekv + lrtcs_coord.Z(), 2)) - R_ekv;
}

Point3D LRTCS::getLRTC() const
        { return lrtcs_coord; }

WGS84 LRTCS::getRlsCoord() const
        { return rls_coord; }

void LRTCS::setLRTC(const Point3D &lrtcs_c)
        { lrtcs_coord = lrtcs_c; }

void LRTCS::setRlsCoord(const WGS84 &rls_c)
        { rls_coord = rls_c; }

LRTCS& LRTCS::operator=(const LRTCS &coord)
{
    this->setLRTC(coord.lrtcs_coord);
    this->setRlsCoord(coord.rls_coord);
    return *this;
}
LRTCS& LRTCS::operator=(const SpCS &coord)
{
    Point3D sp_c = coord.getSpCS();
    double pr = sp_c.X() * cos(sp_c.Z());
    double dh = pow(pr, 2) / 2 / R_ekv;
    double h = sp_c.X() * sin(sp_c.Z());// + dh;// + coord.getRlsCoord().getGeoC().H(); //geoc or geod?
    lrtcs_coord.setCoord(pr * cos(sp_c.Y()), pr * sin(sp_c.Y()), h);
    rls_coord = coord.getRlsCoord();
    return *this;
}


SpCS::SpCS(const LRTCS& coord)
{
    Point3D lrtcs_c = coord.getLRTC();
    double azim = 0;
    double pr = sqrt(pow(lrtcs_c.X(), 2) + pow(lrtcs_c.Y(), 2));
     if(pr != 0)
     {
        if( lrtcs_c.X() >= 0 )
            azim = acos(lrtcs_c.X() / pr);
        else
            azim = -acos(lrtcs_c.X() / pr) + 2*PI;
     }
    else azim = 0; // если точки совпадают
    double dh = pow(pr, 2) / 2 / R_ekv;
    double r = sqrt(pow(pr, 2) + pow(dh - lrtcs_c.H(), 2));

    double ang = 0;
    if(r != 0){
        if( dh <= lrtcs_c.H())
            ang = acos(pr / r);
        else
            ang = -acos(pr / r);
    }
    else ang = 0; // усли точки совпадают
    sp_coord.setCoord(r, azim, ang);
    rls_coord = coord.getRlsCoord();
}

void SpCS::setSpCS(const Point3D &spcs_c)
        { sp_coord = spcs_c; }

void SpCS::setRlsCoord(const WGS84 &rls_c)
        { rls_coord = rls_c; }

Point3D SpCS::getSpCS() const
        { return sp_coord; }

WGS84 SpCS::getRlsCoord() const
        { return rls_coord; }

SpCS::SpCS(const WGS84& coord, const WGS84& rls_c)
{
    LRTCS lr(coord, rls_c);
    SpCS sp(lr);
    sp_coord = sp.getSpCS();
    rls_coord = sp.getRlsCoord();
}

SpCS& SpCS::operator=(const LRTCS& coord)
{
    SpCS sp(coord);
    this->setSpCS(sp.getSpCS());
    this->setRlsCoord(sp.getRlsCoord());
    return *this;
}
SpCS& SpCS::operator=(const SpCS &coord)
{
    this->setSpCS(coord.getSpCS());
    this->setRlsCoord(coord.getRlsCoord());
    return *this;
}


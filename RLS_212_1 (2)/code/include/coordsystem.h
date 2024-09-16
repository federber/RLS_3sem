#ifndef COORDSYSTEM_H
#define COORDSYSTEM_H

#include <cmath>

#include "points.h"

class SK42;
class WGS84;
class PZ90_11;

class LRTCS;
class SpCS;

namespace sk42 // константы для СК42 и переводов относительно неё
{
static constexpr double a{6378245}; // большая полуось
static constexpr double b{6356863}; // малая полуось
static constexpr double f{298.3}; // величина сжатия в минус первой степени

static constexpr double m_pz90_11{-0.228}; // масштабный параметр трансформирования для перехода между ПЗ-90.11 и СК42
const Point3D delta_pz90_11{23.557, -140.844, -79.778}; // смещение относительно СК42
const Point3D w_pz90_11{SecToRad(-0.00230), SecToRad(-0.34646), SecToRad(-0.79421)}; // параметры для матрицы перехода из ПЗ-90.11 в СК42
}

namespace pz90_11 // константы для ПЗ-90.11
{
static constexpr double a{6378136}; // большая полуось
static constexpr double b{6356751.3618}; // малая полуось
static constexpr double f{298.25784}; // величина сжатия в минус первой степени
}

namespace wgs84 // константы для WGS84 и переводов относительно неё
{
static constexpr double m_wgs{0}; // масштабный параметр трансформирования для перехода между WGS84 и WGS84
const Point3D delta_wgs{0, 0, 0}; // смещение относительно WGS84
const Point3D w_wgs{0, 0, 0}; // параметры для матрицы перехода из WGS84 в WGS84

static constexpr double m_sk42{-0.22}; // масштабный параметр трансформирования для перехода между СК42 и WGS84
const Point3D delta_sk42{23.57, -140.95, -79.8}; // смещение относительно WGS84
const Point3D w_sk42{SecToRad(0), SecToRad(-0.35), SecToRad(-0.79)}; // параметры для матрицы перехода из СК42 в WGS84

static constexpr double a{6378137}; // большая полуось
static constexpr double b{6356752.31424518}; // малая полуось
static constexpr double f{298.257223563}; // величина сжатия в минус первой степени
}



struct Ellipsoid
{
    // принимает большую, малую полуось и коэффициент сжатия в -1 степени
    Ellipsoid(double maj, double min, double compar);
    const double major_axis; // большая полуось
    const double minor_axis; // малая полуось
    const double comprassion; // величина сжатия
    const double e_1; // первый эксцентреситет
    const double e_2; // второй эксцентреситет
    void print() const; // -------debug-------
};

class CoordSystem
{
public:
    // принимает геоцентрические координаты, матрицу трансформирования и параметры эллипса
    CoordSystem(const Point3D&coord, const std::vector<double>& ellipsoid_par);
    Point3D getGeoD() const; // возвращает геодезические координаты
    Point3D getGeoC() const; // возвращает геоцентрические координаты
    Ellipsoid getEllipsoid() const;
    void setGeoD(const Point3D& coord); // устанавливает геодезические координаты
    void setGeoC(const Point3D& coord); // устанавливает геоцентрические координаты

    virtual CoordSystem& operator= (const SK42&) = 0;
    virtual CoordSystem& operator= (const WGS84&) = 0;
    virtual CoordSystem& operator= (const PZ90_11& ) = 0;


    void print() const; // ------debug----------
protected:
    Ellipsoid ellipsoid;

    Point3D geoD; // геодезические координаты
    Point3D geoC{0, 0, 0}; // п/у геоцентрические координаты

    double merid_rad; // радиус кривизны меридиана
    double vertic_rad; // радиус кривизны вертикала
};

class SK42 : public CoordSystem
{
public:
    SK42(const Point3D& coord);
    SK42();
    SK42& operator= (const WGS84& wgs) override;
    SK42& operator= (const PZ90_11& pz) override;
    SK42& operator= (const SK42& sk) override;
};

class WGS84 : public CoordSystem
{
public:
    WGS84(const Point3D& coord);
    WGS84();
    WGS84& operator= (const SK42& sk) override;
    WGS84& operator= (const PZ90_11& pz) override;

    WGS84& operator= (const WGS84& wgs) override;
};

class PZ90_11 : public CoordSystem
{
public:
    PZ90_11(const Point3D& coord);
    PZ90_11();
    PZ90_11& operator= (const SK42& sk) override;
    PZ90_11& operator= (const WGS84& wgs) override;
    PZ90_11& operator= (const PZ90_11& pz) override;

};


class LRTCS // местная прямоугольная топоцентрическая система координат
{
public:
    LRTCS(): lrtcs_coord{Point3D{0,0,0}}, rls_coord{{0,0,0}}{};
    LRTCS(const WGS84& coord, const WGS84& rls_c);
    Point3D getLRTC() const;
    WGS84 getRlsCoord() const;
    void setLRTC(const Point3D& lrtcs_c);
    void setRlsCoord(const WGS84& rls_c);
    LRTCS& operator= (const LRTCS& coord);
    LRTCS& operator= (const SpCS& coord);
    double get_H() const;

private:
    Point3D lrtcs_coord;    // координаты в местной прямоугольной СК
    WGS84 rls_coord;  // координаты РЛС
};


class SpCS  // сферическая система координат
{
public:
    SpCS(): sp_coord{Point3D{0,0,0}}, rls_coord{{0,0,0}}{};
    SpCS(const LRTCS& coord);
    SpCS(const WGS84& coord, const WGS84& rls_c);

    Point3D getSpCS() const;
    WGS84 getRlsCoord() const;
    void setSpCS(const Point3D& spcs_c);
    void setRlsCoord(const WGS84& rls_c);
    SpCS& operator= (const SpCS& coord);
    SpCS& operator= (const LRTCS& coord);

private:
    Point3D sp_coord;   // координаты в сферической СК радиус,азимут,угол места
    WGS84 rls_coord;  // координаты РЛС
};

#endif // COORDSYSTEM_H

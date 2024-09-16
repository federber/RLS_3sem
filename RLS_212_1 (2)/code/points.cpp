#include "points.h"

double SecToRad(const double x)
{ return x * 0.0000048; }

double DegToRad(const double deg)
{ return (deg / 180.0 * PI); }

double RadToDeg(const double rad)
{return (rad / PI * 180.0); }



Point3D::Point3D()
{
    x = 0.0;
    y = 0.0;
    z = 0.0;
}

Point3D::Point3D(double xx, double yy, double zz)
{
    x = xx;
    y = yy;
    z = zz;
}
Point3D::Point3D(const Point3D& point)
{
    x = point.x;
    y = point.y;
    z = point.z;
}

void Point3D::setCoord(double x, double y, double z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

void Point3D::getCoord()
{
    std::cout<<x<<", "<<y<<", "<<z<<std::endl;
}

double Point3D::X() const {return x;}
double Point3D::Y() const {return y;}
double Point3D::Z() const {return z;}

double Point3D::B() const {return x;}
double Point3D::L() const {return y;}
double Point3D::H() const {return z;}

bool Point3D::operator== (const Point3D& point)
{
    if (x == point.X() && y == point.Y() && z == point.Z())
        return true;
    return false;
}

bool Point3D::operator !=(const Point3D& point)
{ return !(*this == point); }

Point3D& Point3D::operator*= (double a)
{
    x *= a;
    y *= a;
    z *= a;
    return *this;
}

Point3D operator* (const Point3D& point, double a)
{
    Point3D copy = point;
    copy *= a;
    return copy;
}

Point3D operator* (double a, const Point3D& point)
{
    Point3D copy = point;
    copy *= a;
    return copy;
}

Point3D& Point3D::operator+= (const Point3D& point)
{
    x += point.x;
    y += point.y;
    z += point.z;
    return *this;
}

Point3D Point3D::operator+ (const Point3D& point)
{
    Point3D copy = *this;
    copy += point;
    return copy;
}

Point3D& Point3D::operator-= (const Point3D& point)
{
    x -= point.x;
    y -= point.y;
    z -= point.z;
    return *this;
}

Point3D Point3D::operator- (const Point3D& point)
{
    Point3D copy = *this;
    copy += point;
    return copy;
}

Point3D& Point3D::operator*= (const Point3D& point)
{
    x *= point.x;
    y *= point.y;
    z *= point.z;
}

Point3D Point3D::operator* (const Point3D& point)
{
    Point3D copy = *this;
    copy *= point;
    return copy;
}

Point3D Point3D::to_radian() const
{ return Point3D(x * PI / 180, y * PI / 180, z); }

Point3D Point3D::to_degree() const
{ return Point3D(x * 180 / PI, y * 180 / PI, z); }

std::ostream& operator<<(std::ostream& ost, Point3D point)
{
    return ost << point.X() << " " << point.Y() << " " << point.Z();
}

Matrix::Matrix()
{
    a = nullptr;
    n = 0;
    m = 0;
}

Matrix::Matrix(const std::vector<double>& trans_par, int nn, int mm)
{
    n = nn;
    m = mm;
    if(a) delete[] a;
    a = new double[n * m];
    memcpy(a, trans_par.data(), n * m * sizeof(double));
}

Matrix::Matrix(const Matrix& matr)
{
    n = matr.N();
    m = matr.M();
    if(a) delete[] a;
    a = new double[n * m];
    memcpy(a, matr.pointer(), n * m * sizeof(double));
}

Matrix::~Matrix()
{
    if (a)
        delete[] a;
}

void Matrix::printMatrix() const
{
    for(int y = 0; y < n; ++y)
    {
        for(int x = 0; x < m; ++x)
            std::cout << a[y * m + x] << ' ';
        std::cout << std::endl;
    }
}

void Matrix::set(const std::vector<double>& trans_par, int nn, int mm)
{
    if (a) delete[] a;
    n = nn;
    m = mm;
    a = new double[n * m];
    memcpy(a, trans_par.data(), n * m * sizeof(double));
}

int Matrix::size()
{
    return n * m;
}

int Matrix::N() const
{
    return n;
}

int Matrix::M() const
{
    return m;
}

double* Matrix::pointer() const
{
    return a;
}

double* Matrix::operator[](int t) const
{
    return a + m * t;
}

Matrix& Matrix::operator*= (double num)
{
    for (int i = 0; i < n * m; ++i)
            a[i] *= num;
    return *this;
}

Matrix Matrix::operator* (double num)
{
    Matrix copy = *this;
    copy *= num;
    return copy;
}

void Matrix::operator =(const Matrix& matr)
{
    n = matr.N();
    m = matr.M();
    if(a) delete[] a;
    a = new double[n * m];
    memcpy(a, matr.pointer(), n * m * sizeof(double));
}

Point3D operator* (const Point3D& point, const Matrix& matr)
{
    Point3D copy;
    copy.setCoord(matr[0][0] * point.X() + matr[0][1] * point.Y() + matr[0][2] * point.Z(),
                  matr[1][0] * point.X() + matr[1][1] * point.Y() + matr[1][2] * point.Z(),
                  matr[2][0] * point.X() + matr[2][1] * point.Y() + matr[2][2] * point.Z());
    return copy;
}

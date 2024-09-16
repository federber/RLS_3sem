#ifndef POINTS_H
#define POINTS_H

#include <iostream>
#include <vector>
#include <cstring>

#define PI 3.14159265358979323846

double SecToRad(const double x);  // переводит из угловых секунд в радианы (нужно для создания матрицы перехода)
double DegToRad(const double deg);  // переводит из градусов в радианы
double RadToDeg(const double rad);  // переводит из радиан в градусы

class Point3D
{
private:
    double x;
    double y;
    double z;
public:
    Point3D();                                    // конструктор по умолчанию
    Point3D(double xx, double yy, double zz);     // конструктор от трех точек
    Point3D(const Point3D& point);                // конструктор от точки 3D

    void setCoord(double x, double y, double z);  // установка координат
    void getCoord();                              // печать трех координат

    double X() const;                             // x
    double Y() const;                             // y
    double Z() const;                             // z

    double B() const;                                   // x
    double L() const;                                   // y
    double H() const;                                   // z

    Point3D to_radian() const; // convert degree B, L, H to radian
    Point3D to_degree() const; // convert radian B, L, H to degree

    Point3D& operator*= (double a);               // умножение точки 3D на число *=

    Point3D& operator+= (const Point3D& point);   // сложение точки 3D c точкой 3D +=
    Point3D operator+ (const Point3D& point);     // сложение точки 3D c точкой 3D +

    Point3D& operator-= (const Point3D& point); // вычитание точки 3D с точкой 3D -=
    Point3D operator- (const Point3D& point); // вычитание точки 3D с точкой 3D -

    Point3D& operator*= (const Point3D& point);   // умножение точки 3D на точку 3D *=
    Point3D operator* (const Point3D& point);     // умножение точки 3D на точку 3D *

    bool operator== (const Point3D& point);
    bool operator!= (const Point3D& point);
};

Point3D operator* (const Point3D& point, double a); // умножение точки 3D на число *
Point3D operator* (double a, const Point3D& point); // умножение числа на точку 3D *

std::ostream& operator<<(std::ostream& ost, Point3D point);


class Matrix
{
private:
    double *a = nullptr;                                                      // это указатель на область данных
    int m = 0;                                                      // размер n - по вертикали
    int n = 0;                                                      // размер m - по горизонтали
public:
    Matrix();                                                       // конструктор по умолчанию
    Matrix(const std::vector<double>& trans_par, int nn, int mm);   // конструктор от вектора
    Matrix(const Matrix& matr);                                     // конструктор от матрицы
    ~Matrix();

    void printMatrix() const;                                             // печать матрицы
    void set(const std::vector<double>& trans_par, int nn, int mm); // инициализация массивом
    int size();                                                     // размер матрицы
    int N() const;                                                  // размер n - по вертикали
    int M() const;                                                  // размер m - по горизонтали
    double* pointer() const;                                        // указатель на первый элемент матрицы

    double* operator[](int t) const;                                // обращение к элементу
    Matrix& operator*= (double num);                                // умножение матрицы на число *=
    Matrix operator* (double num);                                  // умножение матрицы на число *

    Point3D operator* (const Point3D& point);                       // матрицу умножить на Point3D

    void operator= (const Matrix& matrix);              // оператор копирования
};

Point3D operator* (const Point3D& point, const Matrix& matr);       // Point3D умножить на матрицу


#endif // POINTS_H




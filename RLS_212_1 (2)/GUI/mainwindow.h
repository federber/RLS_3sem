#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qcustomplot.h>
#include <vector>

#include "coordsystem.h"
#include "points.h"
#include "reading_parser.h"
#include "rls_evaluator.h"
#include "rls_utils.h"




QT_BEGIN_NAMESPACE

namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:



    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void draw_table(QString type);
    void save_to_file(QString type);

    void on_pushButton_2_clicked();


    void on_pushButton_4_clicked();

    void handleMousePress(QMouseEvent *event);



    void on_pushButton_evaluation_clicked();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();


    void on_draw_etalon_data_clicked();

    void on_draw_poi_data_clicked();

private:
    bool draw_poi = false;
    bool draw_etl = false;
    std::map<std::size_t, std::vector<std::pair<double,double>>> top_data;
    std::map<std::size_t, std::vector<Point3D>> top_data1;
    std::map<std::size_t, std::vector<std::pair<double,double>>> poi_res;
    std::map<std::size_t, std::vector<GeoData>> sph_data;
    std::map<std::size_t, std::vector<GeoData>> temp_poi;
    Ui::MainWindow *ui;

    bool check_trace(std::map<std::size_t, std::vector<std::pair<double,double>>> &trace, std::size_t trace_number);

};


#endif // MAINWINDOW_H

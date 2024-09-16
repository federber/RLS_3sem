#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLineEdit>
#include <QComboBox>
#include <QMessageBox>
#include <QString>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QTableWidget>
#include <QVector>
#include <QColor>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include "rls_utils.h"
#include "coordsystem.h"
#include "points.h"
#include "rls_evaluator.h"
#include "reading_parser.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->line_edit_etalon_traces->setReadOnly(true);
    ui->line_edit_array_coord_points->setReadOnly(true);
    ui->lineEdit->setReadOnly(true);
    ui->lineEdit_2->setReadOnly(true);
    ui->lineEdit_3->setReadOnly(true);
    ui->lineEdit_4->setReadOnly(true);
    ui->lineEdit_5->setReadOnly(true);
    ui->lineEdit_6->setReadOnly(true);
    ui->lineEdit_7->setReadOnly(true);
    ui->lineEdit_8->setReadOnly(true);
    ui->lineEdit_9->setReadOnly(true);
    ui->lineEdit_10->setReadOnly(true);
    ui->lineEdit_11->setReadOnly(true);
    ui->lineEdit_12->setReadOnly(true);
    ui->lineEdit_13->setReadOnly(true);
    ui->lineEdit_14->setReadOnly(true);
    ui->lineEdit_15->setReadOnly(true);
    ui->lineEdit_16->setReadOnly(true);
    ui->lineEdit_17->setReadOnly(true);
    ui->lineEdit_18->setReadOnly(true);
    ui->lineEdit_19->setReadOnly(true);
    connect(ui->plot, SIGNAL(mousePress(QMouseEvent*)), SLOT(handleMousePress(QMouseEvent*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}
QVector<double> QSList_to_QV(QStringList list)
{
    QVector<double> vect;
    for (int i = 0; i < list.size(); ++i)
        vect.push_back(list[i].toDouble());
    return vect;
}

bool MainWindow::check_trace(std::map<std::size_t, std::vector<std::pair<double, double>>> &trace, std::size_t trace_number)
{
    for (auto &item : trace)
    {
        if (trace_number == item.first)
            return true;
    }
    return false;
}

void MainWindow::handleMousePress(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        // Получите координаты точки, на которую произошло нажатие
        double x0 = ui->plot->xAxis->pixelToCoord(event->pos().x());
        double y0 = ui->plot->yAxis->pixelToCoord(event->pos().y());

        // Ваш код для добавления координат в таблицу
        // Например, если у вас есть экземпляр таблицы с именем "tableWidget":
        QTableWidgetItem *itemX = new QTableWidgetItem(QString::number(x0));
        QTableWidgetItem *itemY = new QTableWidgetItem(QString::number(y0));

        int row = ui->tableWidget_2->rowCount();
        ui->tableWidget_2->insertRow(row);
        ui->tableWidget_2->setItem(row, 0, itemX);
        ui->tableWidget_2->setItem(row, 1, itemY);

    }
}





void MainWindow::draw_table(QString type) //рисует таблицу по выданному файлу(перед этим конвертирует его)
{
    //открываем файл
    QString fileName = QFileDialog::getOpenFileName(this, "Выберите файл", "", "Текстовые файлы (*.txt);;Все файлы (*)");
    if (fileName.isEmpty())
    {
        return;
    }

    QFile qfile(fileName);
    if (!qfile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл");
        return;
    }


    //ЗДЕСЬ ПРИДЕТСЯ КОНВЕРИТРОВАТЬ ЭТОТ ФАЙЛ, С ПОМОЩЬЮ МАТЕМАТИЧЕСКИХ ФУНКЦИЙ

    Coord_converter conv;
    conv.set_rls_coodr(55.89558/180*PI, 41.206408/180*PI, 177);

    if (type == "G-T")
    {
        std::pair<std::string, std::string> type = {ui->comboBox->currentText().toStdString(), "WGS84"};
        conv.set_inp_par_type(type);
        auto inp = parser_etalon(readFileData(fileName.toStdString()));
        conv.set_etalon_traces(inp);
        sph_data = conv.get_etalon_traces_spher();
        top_data = conv.conv_from_sph_to_tpc(sph_data);



        // Очистка таблицы перед загрузкой новых данных
        ui->tableWidget->clear();
        ui->tableWidget->setRowCount(0);
        ui->tableWidget->setColumnCount(0);

        // Заполнение таблицы
        int column = 0;

        ui->tableWidget->setColumnCount(top_data.size() * 2);
        bool flag_set = false;

        for (auto &item : top_data) {
            if (flag_set == false) {
                ui->tableWidget->setRowCount(item.second.size());
                flag_set = true;
            }
            QString traceHeaderX = "Trace number " + QString::number(item.first) + " X";
            QString traceHeaderY = "Trace number " + QString::number(item.first) + " Y";

            QTableWidgetItem *itemKey1 = new QTableWidgetItem(traceHeaderX);
            ui->tableWidget->setHorizontalHeaderItem(column, itemKey1);

            QTableWidgetItem *itemKey2 = new QTableWidgetItem(traceHeaderY);
            ui->tableWidget->setHorizontalHeaderItem(column + 1, itemKey2);

            for (std::size_t i = 0; i < item.second.size(); ++i) {
                QTableWidgetItem *itemX = new QTableWidgetItem(QString::number(item.second[i].first));
                ui->tableWidget->setItem(i, column, itemX);

                QTableWidgetItem *itemY = new QTableWidgetItem(QString::number(item.second[i].second));
                ui->tableWidget->setItem(i, column + 1, itemY);
            }
            column += 2;
            if (item.second.size() > ui->tableWidget->rowCount())
                ui->tableWidget->setRowCount(item.second.size());
        }

    }
    else if(type == "T-S")
    {
        std::pair<std::string, std::string> type = {ui->comboBox->currentText().toStdString(), "WGS84"};
        conv.set_inp_par_type(type);
        auto inp = parser_point(readFileData(fileName.toStdString()));
        top_data1 = conv.conv_from_tpc3_to_sph(inp);

        // Очистка таблицы перед загрузкой новых данных
        ui->tableWidget->clear();
        ui->tableWidget->setRowCount(0);
        ui->tableWidget->setColumnCount(0);

        // Заполнение таблицы
        int column = 0;

        ui->tableWidget->setColumnCount(top_data1.size() * 2);
        bool flag_set = false;

        for (auto &item : top_data1) {
            if (flag_set == false) {
                ui->tableWidget->setRowCount(item.second.size());
                flag_set = true;
            }
            QString traceHeaderX = "Trace number " + QString::number(item.first) + " Slange_Range";
            QString traceHeaderY = "Trace number " + QString::number(item.first) + " Azimuth";
            QString traceHeaderZ = "Trace number " + QString::number(item.first) + " Elevation";


            QTableWidgetItem *itemKey1 = new QTableWidgetItem(traceHeaderX);
            ui->tableWidget->setHorizontalHeaderItem(column, itemKey1);

            QTableWidgetItem *itemKey2 = new QTableWidgetItem(traceHeaderY);
            ui->tableWidget->setHorizontalHeaderItem(column + 1, itemKey2);

            QTableWidgetItem *itemKey3 = new QTableWidgetItem(traceHeaderZ);
            ui->tableWidget->setHorizontalHeaderItem(column + 2, itemKey3);

            for (std::size_t i = 0; i < item.second.size(); ++i) {
                QTableWidgetItem *itemX = new QTableWidgetItem(QString::number(item.second[i].X()));
                ui->tableWidget->setItem(i, column, itemX);

                QTableWidgetItem *itemY = new QTableWidgetItem(QString::number(item.second[i].Y()));
                ui->tableWidget->setItem(i, column + 1, itemY);

                QTableWidgetItem *itemZ = new QTableWidgetItem(QString::number(item.second[i].Z()));
                ui->tableWidget->setItem(i, column + 2, itemZ);
            }
            column += 3;
        }
    }
    else if(type == "KT_array")
    {
        temp_poi = parser_geodata(readFileData(fileName.toStdString()));
        poi_res = conv.conv_from_sph_to_tpc(temp_poi);

        // Очистка таблицы перед загрузкой новых данных
        ui->tableWidget_4->clear();
        ui->tableWidget_4->setRowCount(0);
        ui->tableWidget_4->setColumnCount(0);

        // Заполнение таблицы
        int column = 0;

        ui->tableWidget_4->setColumnCount(poi_res.size() * 2);
        bool flag_set = false;

        for (auto &item : poi_res) {
            if (flag_set == false) {
                ui->tableWidget_4->setRowCount(item.second.size());
                flag_set = true;
            }
            QString traceHeaderX = "Trace number " + QString::number(item.first) + " X";
            QString traceHeaderY = "Trace number " + QString::number(item.first) + " Y";

            QTableWidgetItem *itemKey1 = new QTableWidgetItem(traceHeaderX);
            ui->tableWidget_4->setHorizontalHeaderItem(column, itemKey1);

            QTableWidgetItem *itemKey2 = new QTableWidgetItem(traceHeaderY);
            ui->tableWidget_4->setHorizontalHeaderItem(column + 1, itemKey2);

            for (std::size_t i = 0; i < item.second.size(); ++i) {
                QTableWidgetItem *itemX = new QTableWidgetItem(QString::number(item.second[i].first));
                ui->tableWidget_4->setItem(i, column, itemX);

                QTableWidgetItem *itemY = new QTableWidgetItem(QString::number(item.second[i].second));
                ui->tableWidget_4->setItem(i, column + 1, itemY);
            }
            column += 2;
            if (item.second.size() > ui->tableWidget_4->rowCount())
                ui->tableWidget_4->setRowCount(item.second.size());
        }

    }


    qfile.close();
}


void MainWindow::save_to_file(QString type)
{
    //открываем файл
    QString fileName = QFileDialog::getOpenFileName(this, "Выберите файл, который хотите конвертировать", "", "Текстовые файлы (*.txt);;Все файлы (*)");
    if (fileName.isEmpty())
    {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл");
        return;
    }

    //ЗДЕСЬ ПРИДЕТСЯ КОНВЕРИТРОВАТЬ ЭТОТ ФАЙЛ, С ПОМОЩЬЮ МАТЕМАТИЧЕСКИХ ФУНКЦИЙ

    Coord_converter conv;
    conv.set_rls_coodr(55.89558/180*PI, 41.206408/180*PI, 177);

    if (type == "G-T")
    {
        std::pair<std::string, std::string> type = {ui->comboBox->currentText().toStdString(), "WGS84"};
        conv.set_inp_par_type(type);
        auto inp = parser_etalon(readFileData(fileName.toStdString()));
        conv.set_etalon_traces(inp);
        sph_data = conv.get_etalon_traces_spher();
        top_data = conv.conv_from_sph_to_tpc(sph_data);

        //сохранение в файл


        QString filePath = QFileDialog::getSaveFileName(nullptr, "Выберите, в какой файл сохранить", "", "Текстовые файлы (*.txt);;Все файлы (*)");
        QFile qfile(filePath);

        if (qfile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream stream(&qfile);

            // Write headers
            for (const auto& entry : top_data)
                stream << "Trace " << entry.first << " X\t" << "Trace " << entry.first << " Y\t";
            stream << "\n";

            // Write data
            int maxRowCount = 0;
            for (const auto& entry : top_data)
                maxRowCount = std::max(maxRowCount, static_cast<int>(entry.second.size()));

            for (int i = 0; i < maxRowCount; ++i)
            {
                for (const auto& entry : top_data)
                {
                    if (i < entry.second.size())
                        stream << entry.second[i].first << "\t" << entry.second[i].second << "\t";
                    else
                        stream << "\t\t"; // Empty cells if no data
                }
                stream << "\n";
            }

            qfile.close();
            file.close();
        } else
            QMessageBox::critical(this, "Ошибка", "Не удалось сохранить в файл");

    }
    else if (type == "T-S")
    {
        std::pair<std::string, std::string> type = {ui->comboBox->currentText().toStdString(), "WGS84"};
        conv.set_inp_par_type(type);
        auto inp = parser_point(readFileData(fileName.toStdString()));
        top_data1 = conv.conv_from_tpc3_to_sph(inp);


        //сохранение в файл


        QString filePath = QFileDialog::getSaveFileName(nullptr, "Выберите, в какой файл сохранить", "", "Текстовые файлы (*.txt);;Все файлы (*)");
        QFile qfile(filePath);

        if (qfile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream stream(&qfile);

            // Write headers
            for (const auto& entry : top_data1)
                stream << "Trace " << entry.first << " Slange_Range\t" << "Trace " << entry.first << " Azimuth\t" << "Trace " << entry.first << " Elevation\t";
            stream << "\n";

            // Write data
            int maxRowCount = 0;
            for (const auto& entry : top_data1)
                maxRowCount = std::max(maxRowCount, static_cast<int>(entry.second.size()));

            for (int i = 0; i < maxRowCount; ++i)
            {
                for (const auto& entry : top_data1)
                {
                    if (i < entry.second.size())
                        stream << entry.second[i].X() << "\t" << entry.second[i].Y() << "\t" << entry.second[i].Z() << "\t";
                    else
                        stream << "\t\t"; // Empty cells if no data
                }
                stream << "\n";
            }

            qfile.close();
            file.close();
        } else
            QMessageBox::critical(this, "Ошибка", "Не удалось сохранить в файл");

    }
    else if(type == "KT_array")
    {
        temp_poi = parser_geodata(readFileData(fileName.toStdString()));
        poi_res = conv.conv_from_sph_to_tpc(temp_poi);

        //сохранение в файл

        QString filePath = QFileDialog::getSaveFileName(nullptr, "Выберите, в какой файл сохранить", "", "Текстовые файлы (*.txt);;Все файлы (*)");
        QFile qfile(filePath);

        if (qfile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream stream(&qfile);

            // Write headers
            for (const auto& entry : poi_res)
                stream << "Trace " << entry.first << " X\t" << "Trace " << entry.first << " Y\t";
            stream << "\n";

            // Write data
            int maxRowCount = 0;
            for (const auto& entry : poi_res)
                maxRowCount = std::max(maxRowCount, static_cast<int>(entry.second.size()));

            for (int i = 0; i < maxRowCount; ++i)
            {
                for (const auto& entry : poi_res)
                {
                    if (i < entry.second.size())
                        stream << entry.second[i].first << "\t" << entry.second[i].second << "\t";
                    else
                        stream << "\t\t"; // Empty cells if no data
                }
                stream << "\n";
            }

            qfile.close();
            file.close();
        } else
            QMessageBox::critical(this, "Ошибка", "Не удалось сохранить в файл");


    }
}





void MainWindow::on_pushButton_2_clicked() //Эталонные трассы ВО   G-T; T-S; G-S
{
    QString selectedOption = ui->comboBox_2->currentText();
    if (selectedOption == "Table G-T_rls"){
        draw_table("G-T");
        ui->label_4->clear();
        ui->label_4->setText("Конвертация «эталонных» координат из глобальной СК в топоцентрическую СК");
    }
    else if (selectedOption == "Table T-S"){
        draw_table("T-S");
        ui->label_4->clear();
        ui->label_4->setText("Конвертация «эталонных» координат из топоцентрической СК в сферическую СК");
    }
    else if (selectedOption == "Save to file G-T_rls"){
        save_to_file("G-T");
        ui->label_4->clear();
        ui->label_4->setText("Конвертация «эталонных» координат из глобальной СК в топоцентрическую СК");
    }
    else if (selectedOption == "Save to file T-S"){
        save_to_file("T-S");
        ui->label_4->clear();
        ui->label_4->setText("Конвертация «эталонных» координат из топоцентрической СК в сферическую СК");
    }

}


void MainWindow::on_pushButton_4_clicked() //Массив КТ
{
    QString selectedOption = ui->comboBox_4->currentText();
    if (selectedOption == "Table S-T")
        draw_table("KT_array");
    else if (selectedOption == "Save to file S-T")
        save_to_file("KT_array");
    ui->label_4->clear();
    ui->label_4->setText("Конвертация выхода ПОИ из сферической СК в топоцентрическую СК");
}



void MainWindow::on_pushButton_evaluation_clicked()
{

    if (!check_trace(top_data, static_cast<std::size_t>(ui->lineEdit_20->text().toULongLong())) || !check_trace(poi_res, static_cast<std::size_t>(ui->lineEdit_21->text().toULongLong())))
        QMessageBox::critical(this, "Ошибка", "Trace not found");
    else
    {
        //в итоге будут отбражены все параметры оценки РЛС
        Evaluator ev;
        ev.setPoiRes(temp_poi);
        ev.setTraces(sph_data);
        std::size_t etl_n = static_cast<std::size_t>(ui->lineEdit_20->text().toULongLong());
        std::size_t poi_n = static_cast<std::size_t>(ui->lineEdit_21->text().toULongLong());
        ev.interpolate(poi_n, etl_n);

        Req_param par;
        par.T_obz = 10;
        par.h_c = 10;
        par.L_sb = 5;
        par.N_obz = 7;
        auto res = ev.getEvalParam(poi_n, par);
        ui->lineEdit->setText(QString::number(res.dist_accur.X()));
        ui->lineEdit_5->setText(QString::number(res.dist_accur.Y()));
        ui->lineEdit_9->setText(QString::number(res.dist_accur.Z()));
        ui->lineEdit_2->setText(QString::number(res.azim_accur.X()));
        ui->lineEdit_6->setText(QString::number(res.azim_accur.Y()));
        ui->lineEdit_10->setText(QString::number(res.azim_accur.Z()));
        ui->lineEdit_3->setText(QString::number(res.aim_ang_accur.X()));
        ui->lineEdit_7->setText(QString::number(res.aim_ang_accur.Y()));
        ui->lineEdit_11->setText(QString::number(res.aim_ang_accur.Z()));
        ui->lineEdit_4->setText(QString::number(res.rad_sp_accur.X()));
        ui->lineEdit_8->setText(QString::number(res.rad_sp_accur.Y()));
        ui->lineEdit_12->setText(QString::number(res.rad_sp_accur.Z()));

        ui->lineEdit_13->setText(QString::number(res.min_dist.first));
        ui->lineEdit_14->setText(QString::number(res.min_dist.second));

        ui->lineEdit_15->setText(QString::number(res.max_dist.first));
        ui->lineEdit_16->setText(QString::number(res.max_dist.second));

        ui->lineEdit_17->setText(QString::number(res.rh_rlz_coef));
    }


}

void MainWindow::on_pushButton_clicked()
{
    if (top_data.size() == 0)
        ui->lineEdit_18->setText("Data not downloaded");
    else
        ui->lineEdit_18->setText("Data downloaded");

    if (poi_res.size() == 0)
        ui->lineEdit_19->setText("Data not downloaded");
    else
        ui->lineEdit_19->setText("Data downloaded");

}

void MainWindow::on_pushButton_3_clicked()
{
    top_data.clear();
    poi_res.clear();

    draw_poi = false;
    draw_etl = false;

    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
    ui->lineEdit_4->clear();
    ui->lineEdit_5->clear();
    ui->lineEdit_6->clear();
    ui->lineEdit_7->clear();
    ui->lineEdit_8->clear();
    ui->lineEdit_9->clear();
    ui->lineEdit_10->clear();
    ui->lineEdit_11->clear();
    ui->lineEdit_12->clear();
    ui->lineEdit_13->clear();
    ui->lineEdit_14->clear();
    ui->lineEdit_15->clear();
    ui->lineEdit_16->clear();
    ui->lineEdit_17->clear();

    ui->tableWidget->clearContents();  // Clears the cell contents
    ui->tableWidget->setRowCount(0);  // Clears the rows
    ui->tableWidget->setColumnCount(0);  // Clears the columns

    ui->tableWidget_4->clearContents();  // Clears the cell contents
    ui->tableWidget_4->setRowCount(0);  // Clears the rows
    ui->tableWidget_4->setColumnCount(0);  // Clears the columns

    ui->label_4->clear();

    //также нужно стереть все графики, которые мы нарисовали по top_data and poi_data
    ui->plot->clearGraphs();
    ui->plot->legend->clear();

    ui->plot->replot();

}



void MainWindow::on_draw_etalon_data_clicked()
{
    if (top_data.size() == 0)
        QMessageBox::critical(this, "Error", "There is no data to draw a graph");
    else if (draw_etl == true)
        QMessageBox::critical(this, "Error", "You need to delete the previous graph, before adding a new one");
    else
    {
        draw_etl = true;
        // Iterate through the map and plot each trace
        for (const auto& entry : top_data) {
            std::size_t traceNumber = entry.first;
            const std::vector<std::pair<double, double>>& points = entry.second;

            // Create a new graph for the trace
            ui->plot->addGraph();
            QCPGraph *graph = ui->plot->graph();

            // Set the trace name for the legend
            graph->setName("Trace " + QString::number(traceNumber) + " etalon");

            // Set the data for the graph
            QVector<double> xData, yData;
            for (const auto& point : points) {
                xData.append(point.first);
                yData.append(point.second);
            }

            graph->setData(xData, yData);

            //style
            QColor Color;
            Color.setRgb(rand() % (255 - 20 + 1) + 20, rand() % (255 - 20 + 1) + 20, rand() % (255 - 20 + 1) + 20);

            QCPScatterStyle scatterStyle;
            scatterStyle.setShape(QCPScatterStyle::ssCircle); // Форма точек
            scatterStyle.setBrush(Color); // Заполнение точек wветом

            ui->plot->graph()->setScatterStyle(scatterStyle);
            ui->plot->graph()->setPen(QPen(Color));

        }

        // Set up the legend
        ui->plot->legend->setVisible(true);
        ui->plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignRight);
        ui->plot->legend->setFont(QFont("Helvetica", 9));
        //ui->plot->legend->setBrush(QColor(255, 255, 255, 150));

        ui->plot->setInteractions(QCP::iRangeZoom | QCP::iRangeDrag);

        // Rescale the axes to fit the data
        ui->plot->rescaleAxes();
        ui->plot->replot();
    }

}

void MainWindow::on_draw_poi_data_clicked()
{
    if (poi_res.size() == 0)
        QMessageBox::critical(this, "Error", "There is no data to draw a graph");
    else if (draw_poi == true)
        QMessageBox::critical(this, "Error", "You need to delete the previous graph, before adding a new one");
    else
    {
        draw_poi = true;
        // Iterate through the map and plot each trace
        for (const auto& entry : poi_res) {
            std::size_t traceNumber = entry.first;
            const std::vector<std::pair<double, double>>& points = entry.second;

            // Create a new graph for the trace
            ui->plot->addGraph();
            QCPGraph *graph = ui->plot->graph();

            // Set the trace name for the legend
            graph->setName("Trace " + QString::number(traceNumber) + " poi");

            // Set the data for the graph
            QVector<double> xData, yData;
            for (const auto& point : points) {
                xData.append(point.first);
                yData.append(point.second);
            }

            graph->setData(xData, yData);

            //style
            QColor Color;
            Color.setRgb(rand() % (255 - 20 + 1) + 20, rand() % (255 - 20 + 1) + 20, rand() % (255 - 20 + 1) + 20);

            QCPScatterStyle scatterStyle;
            scatterStyle.setShape(QCPScatterStyle::ssCircle); // Форма точек
            scatterStyle.setBrush(Color); // Заполнение точек wветом

            ui->plot->graph()->setScatterStyle(scatterStyle);
            ui->plot->graph()->setPen(QPen(Color));

        }

        // Set up the legend
        ui->plot->legend->setVisible(true);
        ui->plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignRight);
        ui->plot->legend->setFont(QFont("Helvetica", 9));
        //ui->plot->legend->setBrush(QColor(255, 255, 255, 150));

        ui->plot->setInteractions(QCP::iRangeZoom | QCP::iRangeDrag);

        // Rescale the axes to fit the data
        ui->plot->rescaleAxes();
        ui->plot->replot();
    }


}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QListWidgetItem>
#include <QPixmap>
#include <QFileDialog>
#include <QTextStream>
#include <QMenuBar>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct Trip {
    QString name;
    QString date;
    qreal cost;
    QString imagePath;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_add_clicked();
    void on_deleteB_clicked();
    void on_search_clicked();
    void on_tableWidget_cellClicked(int row, int column);
    void on_edit_clicked();
    void on_sort_clicked();
    void on_open_triggered();
    void on_saveas_triggered();
    void on_about_triggered();

    void on_tableWidget_cellDoubleClicked(int row, int column);

private:
    Ui::MainWindow *ui;
    std::vector<Trip> trips;

    void updateTable();
    void updateList();
};
#endif // MAINWINDOW_H

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->tableWidget->setColumnCount(4);
    ui->tableWidget->setHorizontalHeaderLabels({"Название", "Дата", "Стоимость, $", "Изображение"});
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::updateTable() {
    ui->tableWidget->setRowCount(trips.size());
    for (int i = 0; i < trips.size(); ++i) {
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(trips[i].name));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(trips[i].date));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(trips[i].cost)));

        QLabel *imageLabel = new QLabel;
        QPixmap pix(trips[i].imagePath);
        imageLabel->setPixmap(pix.scaled(64, 64, Qt::KeepAspectRatio));
        ui->tableWidget->setCellWidget(i, 3, imageLabel);
    }

    updateList();
}

void MainWindow::updateList() {
    ui->listWidget->clear();
    for (const auto& trip : trips)
        ui->listWidget->addItem(trip.name);
}

void MainWindow::on_add_clicked() {
    Trip t;
    t.name = ui->nameEdit->text();
    t.date = ui->dateEdit->text();
    t.cost = ui->costEdit->text().toDouble();
    t.imagePath = QFileDialog::getOpenFileName(this, "Выберите изображение", "", "(*.png *.jpg *.jpeg)");

    trips.push_back(t);
    updateTable();
}

void MainWindow::on_deleteB_clicked() {
    auto selectedItems = ui->tableWidget->selectedItems();
    std::set<int> rowsToDelete;

    for (QTableWidgetItem *item : selectedItems)
        rowsToDelete.insert(item->row());

    std::vector<Trip> newTrips;
    for (int i = 0; i < trips.size(); ++i)
        if (!rowsToDelete.count(i))
            newTrips.push_back(trips[i]);

    trips = std::move(newTrips);
    updateTable();
}

void MainWindow::on_edit_clicked() {
    int row = ui->tableWidget->currentRow();
    if (row < 0 || row >= trips.size()) return;

    trips[row].name = ui->nameEdit->text();
    trips[row].date = ui->dateEdit->text();
    trips[row].cost = ui->costEdit->text().toDouble();
    updateTable();
}

void MainWindow::on_search_clicked() {
    QString query = ui->searchEdit->text().toLower();
    ui->tableWidget->setRowCount(0);
    int row = 0;

    for (const auto& t : trips) {
        if (t.name.toLower().contains(query)) {
            ui->tableWidget->insertRow(row);
            ui->tableWidget->setItem(row, 0, new QTableWidgetItem(t.name));
            ui->tableWidget->setItem(row, 1, new QTableWidgetItem(t.date));
            ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(t.cost)));

            QLabel *img = new QLabel;
            img->setPixmap(QPixmap(t.imagePath).scaled(64, 64, Qt::KeepAspectRatio));
            ui->tableWidget->setCellWidget(row, 3, img);
            ++row;
        }
    }
}

void MainWindow::on_tableWidget_cellClicked(int row, int) {
    if (row < 0 || row >= trips.size()) return;
    ui->nameEdit->setText(trips[row].name);
    ui->dateEdit->setText(trips[row].date);
    ui->costEdit->setText(QString::number(trips[row].cost));
}

void MainWindow::on_saveas_triggered() {
    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить", "", "(*.txt)");
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;

    QTextStream out(&file);
    out << trips.size() << '\n';
    for (const Trip& t : trips)
        out << t.name << '\n' << t.date << '\n' << t.cost << '\n' << t.imagePath << '\n';
    file.close();
}


void MainWindow::on_open_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, "Открыть", "", "(*.txt)");
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&file);
    int count;
    in >> count;
    in.readLine();

    trips.clear();
    for (int i = 0; i < count; ++i) {
        Trip t;
        t.name = in.readLine();
        t.date = in.readLine();
        in >> t.cost;
        in.readLine();
        t.imagePath = in.readLine();
        trips.push_back(t);
    }
    file.close();
    updateTable();
}

void MainWindow::on_about_triggered() {
    QMessageBox::information(this, "О программе",
                             "Редактор таблиц под туристические поездки\n"
                             "Название, время, цена, фотка\n"
                             "Изменение элементов через кнопку Изменить \n Антон Орлевич");
}

void MainWindow::on_sort_clicked() {
    bool check = ui->sortcheck->isChecked();
    std::sort(trips.begin(), trips.end(), [check](const Trip& a, const Trip& b) {
        return check ? a.cost > b.cost : a.cost < b.cost;
    });

    updateTable();
}

void MainWindow::on_tableWidget_cellDoubleClicked(int row, int column)
{
    if (column == 3 && row >= 0 && row < trips.size()) {
        QDialog* imgDialog = new QDialog(this);
        imgDialog->setWindowTitle(trips[row].name);
        QLabel* imgLabel = new QLabel(imgDialog);
        imgLabel->setPixmap(QPixmap(trips[row].imagePath).scaled(400, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        QVBoxLayout* layout = new QVBoxLayout;
        layout->addWidget(imgLabel);
        imgDialog->setLayout(layout);
        imgDialog->exec();
    }
}

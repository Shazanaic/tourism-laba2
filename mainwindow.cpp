#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->tableWidget->setColumnCount(5);
    ui->tableWidget->setHorizontalHeaderLabels({"Название", "Дата", "Стоимость, $", "Изображение", "Индекс"});
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->setColumnHidden(4, true);
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
        imageLabel->setPixmap(pix.scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->tableWidget->setCellWidget(i, 3, imageLabel);

        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(i)));
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
    std::set<int> indexDELETOR;

    for (QTableWidgetItem *item : selectedItems) {
        int row = item->row();
        QTableWidgetItem *idItem = ui->tableWidget->item(row, 4);
        if (idItem)
            indexDELETOR.insert(idItem->text().toInt());
    }

    std::vector<Trip> newTrips;
    for (int i = 0; i < trips.size(); ++i)
        if (!indexDELETOR.count(i))
            newTrips.push_back(trips[i]);

    trips = std::move(newTrips);
    updateTable();
}

void MainWindow::on_edit_clicked() {
    int row = ui->tableWidget->currentRow();
    if (row < 0) return;

    QTableWidgetItem* idItem = ui->tableWidget->item(row, 4);
    if (!idItem) return;

    int index = idItem->text().toInt();
    if (index < 0 || index >= trips.size()) return;

    trips[index].name = ui->nameEdit->text();
    trips[index].date = ui->dateEdit->text();
    trips[index].cost = ui->costEdit->text().toDouble();
    updateTable();
}

void MainWindow::on_search_clicked() {
    QString query = ui->searchEdit->text().toLower();
    ui->tableWidget->setRowCount(0);
    int row = 0;

    for (int i = 0; i < trips.size(); ++i) {
        const auto& t = trips[i];
        if (t.name.toLower().contains(query)) {
            ui->tableWidget->insertRow(row);
            ui->tableWidget->setItem(row, 0, new QTableWidgetItem(t.name));
            ui->tableWidget->setItem(row, 1, new QTableWidgetItem(t.date));
            ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(t.cost)));

            QLabel *img = new QLabel;
            img->setPixmap(QPixmap(t.imagePath).scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            ui->tableWidget->setCellWidget(row, 3, img);

            ui->tableWidget->setItem(row, 4, new QTableWidgetItem(QString::number(i)));
            ++row;
        }
    }
}

void MainWindow::on_tableWidget_cellClicked(int row, int) {
    if (row < 0) return;

    QTableWidgetItem* idItem = ui->tableWidget->item(row, 4);
    if (!idItem) return;

    int index = idItem->text().toInt();
    if (index < 0 || index >= trips.size()) return;

    ui->nameEdit->setText(trips[index].name);
    ui->dateEdit->setText(trips[index].date);
    ui->costEdit->setText(QString::number(trips[index].cost));
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
                             "Название, время, цена, фото\n"
                             "Изменение элементов через кнопку 'Изменить'\nАнтон Орлевич");
}

void MainWindow::on_sort_clicked() {
    bool check = ui->sortcheck->isChecked();
    std::sort(trips.begin(), trips.end(), [check](const Trip& a, const Trip& b) {
        return check ? a.cost > b.cost : a.cost < b.cost;
    });

    updateTable();
}

void MainWindow::on_tableWidget_cellDoubleClicked(int row, int column) {
    if (column == 3 && row >= 0) {
        QTableWidgetItem* idItem = ui->tableWidget->item(row, 4);
        if (!idItem) return;
        int index = idItem->text().toInt();
        if (index < 0 || index >= trips.size()) return;

        QDialog* imgDialog = new QDialog(this);
        imgDialog->setWindowTitle(trips[index].name);
        QLabel* imgLabel = new QLabel(imgDialog);
        imgLabel->setPixmap(QPixmap(trips[index].imagePath).scaled(400, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        QVBoxLayout* layout = new QVBoxLayout;
        layout->addWidget(imgLabel);
        imgDialog->setLayout(layout);
        imgDialog->exec();
    }
}

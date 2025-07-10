#include <QtTest>
#include <QPushButton>
#include <QLineEdit>
#include <QTableWidget>
#include <QTextEdit>
#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>
#include <QVBoxLayout>
#include "mainwindow.h"
#include "ui_mainwindow.h"

class tests : public QObject {
    Q_OBJECT

private slots:
    void testAdd();
    void testAbout();
    void testSearch();
    void testEdit();
};

void tests::testAdd() {
    MainWindow w;
    auto* nameEdit  = w.findChild<QLineEdit*>("nameEdit");
    auto* dateEdit  = w.findChild<QLineEdit*>("dateEdit");
    auto* costEdit  = w.findChild<QLineEdit*>("costEdit");
    auto* addButton = w.findChild<QPushButton*>("add");
    QVERIFY(nameEdit && dateEdit && costEdit && addButton);

    nameEdit->setText("Test");
    dateEdit->setText("1/1/1111");
    costEdit->setText("123.1");
    QTimer::singleShot(0, [&]() {
        QWidgetList widgets = QApplication::topLevelWidgets();
        for (QWidget* w : widgets) {
            QDialog* dlg = qobject_cast<QDialog*>(w);
            if (dlg) dlg->reject();
        }
    });

    w.show();
    QTest::mouseClick(addButton, Qt::LeftButton);
    auto* table = w.findChild<QTableWidget*>("tableWidget");
    QVERIFY(table);
    QCOMPARE(table->rowCount(), 1);
}



void tests::testAbout() {
    MainWindow w;
    auto* action = w.findChild<QAction*>("about");
    QVERIFY(action);

    QTimer::singleShot(0, [&]() {
        QWidgetList widgets = QApplication::topLevelWidgets();
        for (QWidget* widget : widgets) {
            if (auto* box = qobject_cast<QMessageBox*>(widget)) {
                QCOMPARE(box->windowTitle(), QString("О программе"));
                QVERIFY(box->text().contains("Редактор таблиц"));
                box->accept();
            }
        }
    });

    w.show();
    action->trigger();
}

void tests::testSearch() {
    MainWindow w;

    Trip t;
    t.name = "Kot na arbuze";
    t.date = "1/2/3456";
    t.cost = 1337;
    t.imagePath = "";
    w.trips.push_back(t);
    Trip t2;
    t2.name = "Yalta";
    t2.date = "1/1/1111";
    t2.cost = 228;
    t2.imagePath = "";
    w.trips.push_back(t2);
    w.updateTable();

    auto* searchEdit = w.findChild<QLineEdit*>("searchEdit");
    auto* searchBtn  = w.findChild<QPushButton*>("search");
    QVERIFY(searchEdit && searchBtn);

    searchEdit->setText("kot");
    QTest::mouseClick(searchBtn, Qt::LeftButton);

    auto* table = w.findChild<QTableWidget*>("tableWidget");
    QVERIFY(table);
    QCOMPARE(table->rowCount(), 1);
}

void tests::testEdit() {
    MainWindow w;
    Trip t;
    t.name = "name111";
    t.date = "01.1.1111";
    t.cost = 555;
    t.imagePath = "";
    w.trips.push_back(t);
    w.updateTable();

    auto* table = w.findChild<QTableWidget*>("tableWidget");
    auto* nameEdit = w.findChild<QLineEdit*>("nameEdit");
    auto* costEdit = w.findChild<QLineEdit*>("costEdit");
    auto* dateEdit = w.findChild<QLineEdit*>("dateEdit");
    auto* editBtn  = w.findChild<QPushButton*>("edit");
    QVERIFY(table && nameEdit && editBtn);
    table->selectRow(0);
    table->setCurrentCell(0, 0);
    w.on_tableWidget_cellClicked(0, 0);

    nameEdit->setText("name2222");
    costEdit->setText("13.37");
    dateEdit->setText("22.12.1337");
    QTest::mouseClick(editBtn, Qt::LeftButton);
    QCOMPARE(w.trips[0].name, QString("name2222"));
    QCOMPARE(w.trips[0].cost, 13.37);
    QCOMPARE(w.trips[0].date, QString("22.12.1337"));
}

QTEST_MAIN(tests)
#include "tests.moc"

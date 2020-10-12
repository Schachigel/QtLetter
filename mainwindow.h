#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QPrinter>

#include <QTextDocument>
#include <QMainWindow>

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
    void doPaint(QPagedPaintDevice*);
    void doPaint(QPrinter* );
    void on_pb1_clicked();
    void on_pb3_clicked();
    void on_pb4_clicked();

private:
    Ui::MainWindow *ui;
    // QPrinter p{QPrinter::PrinterMode::HighResolution};paintRequested

};
#endif // MAINWINDOW_H

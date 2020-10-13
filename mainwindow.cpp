#include <QDebug>
#include <QPdfWriter>
#include <QPrinterInfo>
#include <QPainter>
#include <QTextStream>

#include "mainwindow.h"
#include "ui_mainwindow.h"

double ptFromMm(double mm) {
    return mm *2.83465;
}
double mmFromPt(double pt) {
    return pt *0.35278;
}

void printPrinterInfo(QPrinter& p)
{
    qInfo().noquote() << (p.printerName().isEmpty() ?  "<no name>" : p.printerName());
    qInfo() << "Layout:      " << p.pageLayout();
    qInfo() << "Layout->fRPt " << p.pageLayout().fullRectPoints();
    qInfo() << "pageSizeMM:  " << p.pageSizeMM();
    qInfo() << "pageSize:    " << (0 == p.pageSize() ? "A4" : "not A4");
    qInfo() << "pageRect(px, mm, pt) " << p.pageRect() << p.pageRect(QPrinter::Millimeter) << p.pageRect(QPrinter::Point);
    qInfo() << "paperRect(px)" << p.paperRect();
    qInfo() << "fullPageMode " << (p.fullPage() ? "full page mode" : "NOT full page mode");
    qInfo().noquote() << "resolution   " << p.resolution() << "\n";
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
      , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->ppW, SIGNAL(paintRequested(QPrinter*)), SLOT(doPaint(QPrinter*)));

    ui->txtAdresse->setPlainText(""
                 "<div style='font-family:Verdana; font-size:10pt;'>"
                 "<table border=1 cellspacing=0 border-collapse:collapse;><tr><td>"
                 "<div style='font-size:5pt;'>Absender Adresse  Abs-Platz 9, 62143 Absstadt<p></div>"
                 "An<p>Hans Empfänger<br>Sesamstraße 42<p><big>31234</big> <b>EmpfStadt<b>"
                 "</td></tr></table></div>");
    ui->txtBetreff->setPlainText("<div style='font-size:10pt';> Ein wichtiger Brief</div>");
    ui->txtBody->setPlainText("<div style='font-family:Verdana; font-size:10pt;'>"
                              "<table width=100% border=0 cellspacing=0 border-collapse:collapse;><tr><td>"
                              "Lieber Adalbert,<p style='text-align:justify;'>"
                              "herzlichen Dank bla bla blabla blabal bla bla blabla blabalbla bla blabla blabalbla bla blabla blabal"
                              "<p style='text-align:justify; font-size:9pt'>bla bla blabla blabalbla bla blabla blabalbla bla blabla blabalbla bla blabla blabalbla bla blabla blabalbla bla blabla blabalbla bla blabla blabalbla bla blabla blabalbla bla blabla blabalbla bla blabla blabal"
                              "<p align=justify>bla bla blabla blabalbla bla blabla blabalbla bla blabla blabalbla bla blabla blabalbla bla blabla blabalbla bla blabla blabalbla bla blabla blabalbla bla blabla blabalbla bla blabla blabalbla bla blabla blabalbla bla blabla blabal"
                              "<br><p>Mit freundlichen Grüßen</p>"
                              "<p style='text-align:justify; font-size:167px'> Holger Mairon</p>"
                              "<br><small>Job comment of signer</small></p>"
                              "</td></tr></table></div>");
    ui->txtFusszeile->setPlainText("<div style='font-family:Verdana; font-size:6pt'>"
                                   "<table width=100% border=1 cellspacing=0 border-collapse:collapse;>"
                                   "<tr><td width=33% style='text-align:left'><small>Geschäftsführer: Hugo Hurtig, Julia Rüstig, Eduard Montabaur</small></td>"
                                   "<td width=33% style='text-align:left'><small>Eine Bank Frankfurt<br>IBAN 12939 39943 22030 403<br>BIC xxx-dxx-xx</small></td>"
                                   "<td style='text-align:left'><small>Handelsregister Mannheim<br>HSB Eintrag xxxx.xxxx.xxxx</small></td>"
                                   "</tr></table></div>");

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::doPaint(QPrinter* printer)
{
    qInfo() << "Printing Through Printer";
    doPaint((QPagedPaintDevice*)printer);
}

void MainWindow::doPaint(QPagedPaintDevice* pPaintDevice)
{
    pPaintDevice->setPageMargins(QMarginsF(0., 0., 0., 0.));
    QPageLayout lay =pPaintDevice->pageLayout();
    qInfo() << "paintDevice Layout: " << lay << "\n" << lay.fullRectPoints();
    lay.setUnits(QPageLayout::Unit::Point);
    lay.setMode(QPageLayout::FullPageMode);
    lay.setOrientation(QPageLayout::Orientation::Portrait);
    lay.setPageSize(QPageSize(QPageSize::A4));
    pPaintDevice->setPageLayout(lay);

    qInfo() << "Printing Through PPD";
    QPainter painter;

    double leftMarginPt  = 25. *2.83465;
    double rightMarginPt = 20. *2.83465;
    double bottomMarginPt= 16. *2.83465;

    double foldmark1Y  =ptFromMm(105.);
    double foldmark2Y =ptFromMm(210.);
    double foldmarkLength =ptFromMm(5.);
    double centermarkY =ptFromMm(148.5);
    double centermarkLength =ptFromMm(10);

    double addressFromTop   =ptFromMm(45);
    double aboutFromAddress =ptFromMm(12);
    double textFromAbout    =ptFromMm(12);

    painter.begin(pPaintDevice);
//    double unprintableMarginPt =3.;

    ////////////////////////////////////////
    painter.setWindow(lay.fullRectPoints());
    ////////////////////////////////////////

    QTextDocument td;
    td.documentLayout()->setPaintDevice(pPaintDevice);
    td.setPageSize(QSizeF(lay.paintRectPoints().width(), lay.paintRectPoints().height()));
    td.setDocumentMargin(0.);

    // top / bottom marker
    painter.drawLine(QPointF(0., 0.), QPointF(foldmarkLength, 0.));
    painter.drawLine(QPointF(0., lay.fullRectPoints().height()),
                     QPointF(foldmarkLength, lay.fullRectPoints().height()));

    // Falzmarken, Lochmarke
    painter.drawLine(QPointF(0., foldmark1Y), QPointF(foldmarkLength, foldmark1Y));
    painter.drawLine(QPointF(0., foldmark2Y), QPointF(foldmarkLength, foldmark2Y));
    painter.drawLine(QPointF(0., centermarkY), QPointF(centermarkLength, centermarkY));

    double yPos =addressFromTop;
    painter.save();
    painter.translate(QPointF(leftMarginPt, yPos));
    td.setHtml(ui->txtAdresse->toPlainText());
    td.drawContents(&painter);
    painter.restore();

    yPos +=td.size().height() +aboutFromAddress;
    painter.save();
    painter.translate(QPointF(leftMarginPt, yPos));
    td.setHtml(ui->txtBetreff->toPlainText());
    td.drawContents(&painter);
    painter.restore();

    yPos +=td.size().height() +textFromAbout;
    painter.save();
    painter.translate(QPointF(leftMarginPt, yPos));
    double textWidth = lay.fullRectPoints().width()-rightMarginPt-leftMarginPt;
    td.setTextWidth(textWidth);
    td.setHtml(ui->txtBody->toPlainText());
    td.drawContents(&painter);
    painter.restore();

    td.setHtml(ui->txtFusszeile->toPlainText());
    double footerStartY = lay.fullRectPoints().height()-bottomMarginPt-td.size().height();
    painter.translate(QPointF(leftMarginPt, footerStartY));
    td.drawContents(&painter);

    painter.end();
}

void MainWindow::on_pb1_clicked()
{
    qInfo() << "Print to Preview";
    ui->ppW->updatePreview();

}

void MainWindow::on_pb3_clicked()
{
    qInfo() << "Print to QPdfWriter";
    QPdfWriter pdfw("pdfw_out.pdf");
    doPaint(&pdfw);

}

void MainWindow::on_pb4_clicked()
{
    qInfo() << "Print to QPrinter (pdf mode)";

    QPrinter writer;
//    setupPrinter(writer);
    printPrinterInfo(writer);
    writer.setOutputFileName("out.pdf");
    doPaint((QPrinter*)&writer);
}

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

void setupPrinter(QPrinter& p)
{
    p.setOutputFormat(QPrinter::PdfFormat);
    // Normbrief
    p.setFullPage(false);
    QMarginsF marg(25, 16.9, 10, 16.9);
    p.setPageMargins(marg, QPageLayout::Millimeter);
    printPrinterInfo(p);
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
      , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->ppW, SIGNAL(paintRequested(QPrinter*)), SLOT(doPaint(QPrinter*)));
    ui->txtAdresse->setPlainText(""
                 "<div style='font-family:Verdana; font-size:10pt'>"
                 "<table width=100% border=1 cellspacing=0 border-collapse:collapse;><tr><td>"
                 "<small>Absender Adresse  Abs-Platz 9, 62143 Absstadt<p></small>"
                 "An<p>Hans Empfänger<br>Sesamstraße 42<p><big>31234</big> <b>EmpfStadt<b>"
                 "</td></tr></table></div>");
    ui->txtBody->setPlainText("<div style='font-family:Verdana; font-size:10pt;'>"
                              "<table width=100% border=1 cellspacing=0 border-collapse:collapse;><tr><td>"
                              "Lieber Adalbert,<p style='text-align:justify;'>"
                              "herzlichen Dank bla bla blabla blabal bla bla blabla blabalbla bla blabla blabalbla bla blabla blabal"
                              "<p style='text-align:justify;'>bla bla blabla blabalbla bla blabla blabalbla bla blabla blabalbla bla blabla blabalbla bla blabla blabalbla bla blabla blabalbla bla blabla blabalbla bla blabla blabalbla bla blabla blabalbla bla blabla blabal"
                              "<p align=justify>bla bla blabla blabalbla bla blabla blabalbla bla blabla blabalbla bla blabla blabalbla bla blabla blabalbla bla blabla blabalbla bla blabla blabalbla bla blabla blabalbla bla blabla blabalbla bla blabla blabalbla bla blabla blabal"
                              "<br><p>Mit freundlichen Grüßen</p>"
                              "<p> Holger Mairon</p>"
                              "<br><small>Job comment of signer</small></p>"
                              "</td></tr></table></div>");
    ui->txtFusszeile->setPlainText("<hmtl><head></head><body style='font-family:Verdana; font-size:6pt'>"
                                   "<table width=100% border=1 cellspacing=0 border-collapse:collapse;>"
                                   "<tr><td width=33% style='text-align:left'><small>Geschäftsführer: Hugo Hurtig, Julia Rüstig, Eduard Montabaur</small></td>"
                                   "<td width=33% style='text-align:left'><small>Eine Bank Frankfurt<br>IBAN 12939 39943 22030 403<br>BIC xxx-dxx-xx</small></td>"
                                   "<td style='text-align:left'><small>Handelsregister Mannheim<br>HSB Eintrag xxxx.xxxx.xxxx</small></td>"
                                   "</tr></table></body></html>");

    setupPrinter(p);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::doPaint(QPrinter* p)
{
    doPaint((QPagedPaintDevice*)p);
}

void MainWindow::doPaint(QPagedPaintDevice* p)
{
    QPainter painter;
    painter.begin(p);
    QPageLayout lay = p->pageLayout();
    double leftMg =lay.marginsPoints().left();
    double rightMg =lay.marginsPoints().right();
//    double top =lay.marginsPoints().top();
    double bottomMg = lay.marginsPoints().bottom();
    qInfo() << "Marings (l, r):() " << leftMg << ", " << rightMg << ")";
    ////////////////////////////////////////
    painter.setWindow(lay.fullRectPoints());
    ////////////////////////////////////////

    // top / bottom
    painter.drawLine(QPointF(0., 0.), QPointF(15, 0.));
    painter.drawLine(QPointF(0., lay.fullRectPoints().height()), QPointF(15., lay.fullRectPoints().height()));

    // Falzmarken, Lochmarke
    double pos =ptFromMm(105. );
    painter.drawLine(QPointF(0., pos), QPointF(10., pos));
    pos =ptFromMm(210.);
    painter.drawLine(QPointF(0., pos), QPointF(10., pos));
    pos =ptFromMm(148.5);
    painter.drawLine(QPointF(0., pos), QPointF(15., pos));

    painter.save();
    painter.translate(QPointF(leftMg, ptFromMm(45)));
    tdAddress.drawContents(&painter);
    painter.restore();

    painter.save();
    painter.translate(QPointF(leftMg, ptFromMm(105)));
    double textWidth = lay.fullRectPoints().width()-leftMg-rightMg;
    qInfo() << "TextWidth: " << textWidth;
    tdBody.setTextWidth(textWidth);
    tdBody.drawContents(&painter);
    painter.restore();

    tdFoot.setTextWidth(textWidth);
    double footerHeight = tdFoot.size().height();
    double footerStartY = lay.fullRectPoints().height()-bottomMg-footerHeight;
    painter.translate(QPointF(leftMg, footerStartY));
    tdFoot.drawContents(&painter);

    painter.end();
}

void MainWindow::on_pb1_clicked()
{
    tdAddress.setHtml(ui->txtAdresse->toPlainText());
    tdBody.setHtml(ui->txtBody->toPlainText());
    tdFoot.setHtml(ui->txtFusszeile->toPlainText());
    ui->ppW->updatePreview();

    qInfo() << ui->txtAdresse->toPlainText();
    qInfo() << ui->txtBody->toPlainText();
    qInfo() << ui->txtFusszeile->toPlainText();
}

void MainWindow::on_pb3_clicked()
{
    QPdfWriter pdfw("pdfw_out.pdf");
    doPaint(&pdfw);

}

void MainWindow::on_pb4_clicked()
{
    QPrinter writer((QPrinterInfo)p); // does not work as expected
    setupPrinter(writer);
    printPrinterInfo(writer);
    //writer.setPageMargins(0, 0, 0, 0, QPrinter::Point);
    writer.setOutputFileName("out.pdf");
    doPaint((QPrinter*)&writer);
}

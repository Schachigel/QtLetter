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
    QPageLayout pl( QPageSize(QPageSize::A4), QPageLayout::Portrait, marg, QPageLayout::Millimeter);
    p.setPageLayout(pl);
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
    qInfo() << "Printing Through PPD";
    QPainter painter;

    QPageLayout lay = pPaintDevice->pageLayout();
    qInfo() << "paintDevice Layout: " << lay;
    double leftMg =lay.marginsPoints().left();
    double rightMg =lay.marginsPoints().right();
    double topMg =lay.marginsPoints().top();
    double bottomMg = lay.marginsPoints().bottom();
    qInfo() << "orig. margins (l, r):() " << leftMg << ", " << rightMg << ")";
    pPaintDevice->setPageMargins(QMarginsF(ptFromMm(25), ptFromMm(16), ptFromMm(20), ptFromMm(16)));

    painter.begin(pPaintDevice);
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
    QTextDocument td;
    td.documentLayout()->setPaintDevice(pPaintDevice);
    td.setPageSize(QSizeF(lay.paintRectPoints().width(), lay.paintRectPoints().height()));
    td.setDocumentMargin(0.);

    td.setHtml(ui->txtAdresse->toPlainText());
    td.drawContents(&painter);
    painter.restore();

    painter.save();
    painter.translate(QPointF(leftMg, ptFromMm(105)));
    double textWidth = lay.fullRectPoints().width()-leftMg-rightMg;
    qInfo() << "TextWidth: " << textWidth;
    td.setTextWidth(textWidth);
    td.setHtml(ui->txtBody->toPlainText());
    td.drawContents(&painter);
    painter.restore();

    double footerHeight = td.size().height();
    double footerStartY = lay.fullRectPoints().height()-bottomMg-footerHeight;
    painter.translate(QPointF(leftMg, footerStartY));

    td.setHtml(ui->txtFusszeile->toPlainText());
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

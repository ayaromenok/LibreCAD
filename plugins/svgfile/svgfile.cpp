/*****************************************************************************/
/*  svgfile.cpp - SVG file importer                                          */
/*                                                                           */
/*  Copyright (C) 2011 Rallaz, rallazz@gmail.com                             */
/*  Copyright (C) 2014 cgrzemba, cgrzemba@opencsw.org                        */
/*  Copyright (C) 2018 Andrey Yaromenok, ayaromenok@gmail.com                */
/*                                                                           */
/*  This library is free software, licensed under the terms of the GNU       */
/*  General Public License as published by the Free Software Foundation,     */
/*  either version 2 of the License, or (at your option) any later version.  */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program.  If not, see <http://www.gnu.org/licenses/>.    */
/*****************************************************************************/

#include <QtPlugin>
#include <QPicture>
#include <QPainter>
#include <QMouseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QFormLayout>
#include <QPushButton>
#include <QFileDialog>
#include <QSettings>
#include <math.h>

#include <QMessageBox>

#include "document_interface.h"
#include "svgfile.h"
#include <QDebug>
#include <QXmlStreamReader>
PluginCapabilities SvgFile::getCapabilities() const
{
    PluginCapabilities pluginCapabilities;
    pluginCapabilities.menuEntryPoints
            << PluginMenuLocation("plugins_menu", tr("Read SVG file"));
    return pluginCapabilities;
}

QString SvgFile::name() const
 {
     return (tr("import SVG file"));
 }

void SvgFile::execComm(Document_Interface *doc,
                             QWidget *parent, QString cmd)
{
    Q_UNUSED(cmd);
    svgPunto pdt(parent);
    int result = pdt.exec();
    if (result == QDialog::Accepted)
        pdt.processFile(doc);
}

/*****************************/
svgPunto::svgPunto(QWidget *parent) :  QDialog(parent)
{
    QGridLayout *mainLayout = new QGridLayout;

    QPushButton *btFile = new QPushButton(tr("File..."));
    _edFile = new QLineEdit();

    QDoubleValidator *val = new QDoubleValidator(0);
    val->setBottom ( 0.0 );
    _edScale = new QLineEdit();
    _edScale->setValidator(val);
    _edScale->setText("1.0");
    _scale = 1.0; // save to settings

    QFormLayout *flo = new QFormLayout;
    flo->addRow( btFile, _edFile);
    flo->addRow( tr("Scale:"), _edScale);
    mainLayout->addLayout(flo, 0, 0);

    QHBoxLayout *hBoxLayoutAcceptCancel = new QHBoxLayout;
    QPushButton *btAccept = new QPushButton(tr("Accept"));
    hBoxLayoutAcceptCancel->addStretch();
    hBoxLayoutAcceptCancel->addWidget(btAccept);

    QPushButton *btCancel = new QPushButton(tr("Cancel"));
    hBoxLayoutAcceptCancel->addWidget(btCancel);
    mainLayout->addLayout(hBoxLayoutAcceptCancel, 1, 0);

    setLayout(mainLayout);
    //readSettings();

    connect(btCancel, SIGNAL(clicked()), this, SLOT(reject()));
    connect(btAccept, SIGNAL(clicked()), this, SLOT(checkAccept()));

    connect(btFile, SIGNAL(clicked()), this, SLOT(svgFile()));
}

svgPunto::~svgPunto()
{

}

void svgPunto::processFile(Document_Interface *doc)
{
    _curDoc = doc;
    if (!QFile::exists(_edFile->text()) ) {
        QMessageBox::critical ( this, "svgPunto", QString(tr("The file %1 not exist")).arg(_edFile->text()) );
        return;
    }
    QFile inFile(_edFile->text());
    if (!inFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical ( this, "svgPunto", QString(tr("Can't open the file %1")).arg(_edFile->text()) );
         return;
    }

    QString curLayer = doc->getCurrentLayer();
    processFileSvg(&inFile);
    inFile.close ();

//    QMessageBox::information(this, "Info", QString(tr("%1 objects imported")).arg(cnt) );
    _curDoc = NULL;
}

void svgPunto::svgFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select file"));
    _edFile->setText(fileName);
}
void svgPunto::checkAccept()
{
    accept();
}

void svgPunto::processFileSvg(QFile* file)
{
    qDebug() << "processFileSVG" << file->fileName();

    QXmlStreamReader reader(file);

    if (reader.readNextStartElement()) {
        if (reader.name() == "svg"){
            qDebug() << "svg";
            QXmlStreamAttributes attr(reader.attributes());
            parseSvgAttribs(&attr);
            while(reader.readNextStartElement()){
                parseGroup(&reader);
            }
        }
        else{
            qDebug() << "not a SVG";
            reader.raiseError(QObject::tr("Incorrect file"));
        }
    }

}

void svgPunto::parseGroup(QXmlStreamReader *reader)
{
    if(reader->name() == "g"){
        qDebug() << "parse" << reader->name();
        while(reader->readNextStartElement()){
            if(reader->name() == "line"){
                QXmlStreamAttributes attr(reader->attributes());
                drawLine(&attr);
                //we need only attribs, but need to jump to next element
                reader->readElementText();
            } else if(reader->name() == "path"){
                QXmlStreamAttributes attr(reader->attributes());
                drawPath(&attr);
                reader->readElementText();
            } else if(reader->name() == "circle"){
                QXmlStreamAttributes attr(reader->attributes());
                drawCircle(&attr);
                reader->readElementText();
            }
            else if (reader->name() == "g")
            {
                parseGroup(reader);
            }
            else {
                reader->skipCurrentElement();
                qDebug() << "skip2" << reader->name();

            }
        }
    }
    else{
        reader->skipCurrentElement();
        qDebug() << "skip" << reader->name();
    }

}

void svgPunto::drawLine(QXmlStreamAttributes *attr)
{
    QPointF p1, p2;
    if (4 == attr->length()){
        p1.setX(attr->at(0).value().toFloat());
        p1.setY(getY(attr->at(1).value().toFloat()));
        p2.setX(attr->at(2).value().toFloat());
        p2.setY(getY(attr->at(3).value().toFloat()));
        _curDoc->addLine(&p1, &p2);
        qDebug() << "line" << p1 << p2;
    } else {
        qDebug() << "something went wrong";
    }
}

void svgPunto::drawCircle(QXmlStreamAttributes *attr)
{
    QPointF p1;
    if (3 == attr->length()){
        p1.setX(attr->at(0).value().toFloat());
        p1.setY(getY(attr->at(1).value().toFloat()));
        _curDoc->addCircle(&p1, attr->at(2).value().toFloat());
        qDebug() << "circle" << p1 << "radius" << attr->at(2).value().toFloat();
    } else {
        qDebug() << "something went wrong";
    }
}

void svgPunto::drawPath(QXmlStreamAttributes *attr)
{
   // all path is one attibute
    qDebug() << "todo - Mirror by vertical!!!! ";
    if (1 == attr->length()){
        qDebug() << "path" << attr->at(0).value();
        const QString* strPath = attr->at(0).value().string();
        QStringList strlPath = strPath->split(" ");
        //Mxx,yy - moveTo, Lxx,yy - lineTo, Axx,yy xx,yy xx,yy - arc,Z - ?

    } else {
        qDebug() << "something went wrong";
    }
}

void
svgPunto::parseSvgAttribs(QXmlStreamAttributes *attr)
{
    for (int i=0; i<attr->size(); i++){
        if (attr->at(i).name() == "width") {
            qDebug() << "width" << attr->at(i).value();
        } else if (attr->at(i).name() == "height") {
            QString value(attr->at(i).value().toString());
            value.chop(2);
            _height = value.toFloat();
            qDebug() << "height" << _height << "mm";
        } else if (attr->at(i).name() == "viewBox") {
            qDebug() << "viewbox" << attr->at(i).value();
        } else {
            qDebug() << "skipped: " << attr->at(i).name();
        }
    }
}

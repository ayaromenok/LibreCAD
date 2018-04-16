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

}

svgPunto::~svgPunto()
{

}

void svgPunto::processFile(Document_Interface *doc)
{

}

#include <CGAL/Qt/GraphicsViewNavigation.h>
#include <QApplication>
#include <QLabel>
#include <QFile>
#include <QMenu>
#include <QMenuBar>
#include <QAction>
#include <QMessageBox>
#include <QStatusBar>
#include <QGraphicsView>
#include <QGLWidget>
#include <QTextStream>
#include <QSettings>
#include <QFileInfo>

#include <CGAL/Qt/DemosMainWindow.h>

namespace CGAL {
namespace Qt {

DemosMainWindow::DemosMainWindow(QWidget * parent, ::Qt::WindowFlags flags)
  : QMainWindow(parent, flags),
    maxNumRecentFiles(10),
    recentFileActs(maxNumRecentFiles)
{
  // prepare the QLabel xycoord for inclusion in the statusBar()
  xycoord = new QLabel(" -0.00000 , -0.00000 ", this);
  xycoord->setAlignment(::Qt::AlignHCenter);
  xycoord->setMinimumSize(xycoord->sizeHint());
  xycoord->clear();

  actionUse_OpenGL = new QAction(this);
  actionUse_OpenGL->setObjectName("actionUse_OpenGL");
  actionUse_OpenGL->setCheckable(true);
  actionUse_OpenGL->setText(tr("Use &OpenGL"));
  actionUse_OpenGL->setStatusTip(tr("Make Qt use OpenGL to display the graphical items, instead of its native painting system."));
  actionUse_OpenGL->setShortcut(tr("Ctrl+G"));

  actionUse_Antialiasing = new QAction(this);
  actionUse_Antialiasing->setObjectName("actionUse_Antialiasing");
  actionUse_Antialiasing->setCheckable(true);
  actionUse_Antialiasing->setText(tr("Use &anti-aliasing"));
  actionUse_Antialiasing->setStatusTip(tr("Make Qt use anti-aliasing when displaying the graphical items."));
  actionUse_Antialiasing->setShortcut(tr("Ctrl+A"));

  actionAboutCGAL = new QAction(this);
  actionAboutCGAL->setObjectName("actionAboutCGAL");
  actionAboutCGAL->setText(tr("About &CGAL..."));

  actionAbout = new QAction(this);
  actionAbout->setObjectName("actionAbout");
  actionAbout->setText(tr("&About..."));
}

void
DemosMainWindow::addNavigation(QGraphicsView* graphicsView)
{
  navigation = new CGAL::Qt::GraphicsViewNavigation();
  graphicsView->viewport()->installEventFilter(navigation);
  graphicsView->installEventFilter(navigation);
  QObject::connect(navigation, SIGNAL(mouseCoordinates(QString)),
		   xycoord, SLOT(setText(QString)));
  view = graphicsView;
}

void
DemosMainWindow::setupStatusBar()
{
  this->statusBar()->addWidget(new QLabel(this), 1);
  this->statusBar()->addWidget(xycoord, 0);
}

void
DemosMainWindow::setupOptionsMenu(QMenu* menuOptions)
{
  // search for the Options menu
  if(!menuOptions) {
    menuOptions = getMenu("menuOptions", tr("&Options"));
  }

  // if not found, then create it
  if(!menuOptions) {
    menuOptions = new QMenu(this->menuBar());
    menuOptions->setTitle(tr("&Options"));
    this->menuBar()->addAction(menuOptions->menuAction());
    menuOptions->setObjectName("menuOptions");
  }

  if(!menuOptions->isEmpty()) {
    menuOptions->addSeparator();
  }
  menuOptions->addAction(actionUse_OpenGL);
  menuOptions->addAction(actionUse_Antialiasing);
  connect(actionUse_Antialiasing, SIGNAL(toggled(bool)),
          this, SLOT(setUseAntialiasing(bool)));
  connect(actionUse_OpenGL, SIGNAL(toggled(bool)),
          this, SLOT(setUseOpenGL(bool)));
  actionUse_Antialiasing->setChecked(true);
}

void
DemosMainWindow::setUseAntialiasing(bool checked)
{
  view->setRenderHint(QPainter::Antialiasing, checked);
  view->setRenderHint(QPainter::HighQualityAntialiasing, checked);
  statusBar()->showMessage(tr("Antialiasing %1activated").arg(checked?"":"de-"),
                           1000);
}

void
DemosMainWindow::setUseOpenGL(bool checked)
{ 
  if(checked) {
    QGLWidget* new_viewport = new QGLWidget;

    // Setup the format to allow antialiasing with OpenGL:
    // one need to activate the SampleBuffers, if the graphic driver allows
    // this.
    QGLFormat glformat = new_viewport->format();
    glformat.setOption(QGL::SampleBuffers);
    new_viewport->setFormat(glformat);

    view->setViewport(new_viewport);
  }
  else {
    view->setViewport(new QWidget);
  }
  statusBar()->showMessage(tr("OpenGL %1activated").arg(checked?"":"de-"),
                           1000);
  view->viewport()->installEventFilter(navigation);
  view->setFocus();
}

QMenu* 
DemosMainWindow::getMenu(QString objectName, QString title)
{
  QMenu* menu = NULL;

  QString title2 = title;
  title2.remove('&');
  // search if a menu has objectName()==objectName
  menu = this->findChild<QMenu*>(objectName);

  // then search if a menu has title()==title
  if(menu) {
    return menu;
  } else {
    Q_FOREACH(menu, this->findChildren<QMenu*>()) {
      if(menu->title() == title ||
         menu->title() == title2) {
        return menu;
      }
    }
  }
  return NULL;
}

void 
DemosMainWindow::popupAboutBox(QString title, QString html_resource_name)
{
  QFile about_CGAL(html_resource_name);
  about_CGAL.open(QIODevice::ReadOnly);
  QMessageBox mb(QMessageBox::NoIcon,
                 title,
                 QTextStream(&about_CGAL).readAll(),
                 QMessageBox::Ok,
                 this);
  mb.exec();
}

QMenu* DemosMainWindow::getHelpMenu()
{
  QMenu* menuHelp = getMenu("menuHelp", tr("&Help"));
  if(!menuHelp) {
    menuHelp = new QMenu(this->menuBar());
    menuHelp->setTitle(tr("&Help"));
    this->menuBar()->addAction(menuHelp->menuAction());
    menuHelp->setObjectName("menuHelp");
  }
  return menuHelp;
}

void 
DemosMainWindow::addAboutCGAL(QMenu* menuHelp)
{
  if(!menuHelp) {
    menuHelp = getHelpMenu();
  }
  menuHelp->addAction(actionAboutCGAL);

  connect(actionAboutCGAL, SIGNAL(triggered()),
          this, SLOT(popupAboutCGAL()));
}

void 
DemosMainWindow::addAboutDemo(QString htmlResourceName, QMenu* menuHelp)
{
  if(!menuHelp) {
    menuHelp = getHelpMenu();
  }
  menuHelp->addAction(actionAbout);
  aboutHtmlResource = htmlResourceName;

  connect(actionAbout, SIGNAL(triggered()),
          this, SLOT(popupAboutDemo()));
}

void
DemosMainWindow::popupAboutCGAL()
{
  popupAboutBox(tr("About CGAL..."),
                ":/cgal/help/about_CGAL.html");
}

void
DemosMainWindow::popupAboutDemo()
{
  popupAboutBox(tr("About the demo..."),
                aboutHtmlResource);
}

void
DemosMainWindow::setMaxNumberOfRecentFiles(const unsigned int i)
{
  maxNumRecentFiles = i;
  recentFileActs.resize(maxNumRecentFiles);
}

unsigned int 
DemosMainWindow::maxNumberOfRecentFiles() const
{
  return maxNumRecentFiles;
}

void 
DemosMainWindow::openRecentFile_aux()
{
  QAction *action = qobject_cast<QAction *>(sender());
  if (action)
    emit openRecentFile(action->data().toString());
}

void 
DemosMainWindow::addToRecentFiles(QString fileName)
{
  QSettings settings;
  QStringList files = settings.value("recentFileList").toStringList();
  files.removeAll(fileName);
  files.prepend(fileName);
  while (files.size() > (int)maxNumberOfRecentFiles())
    files.removeLast();

  settings.setValue("recentFileList", files);

  updateRecentFileActions();
}

void
DemosMainWindow::addRecentFiles(QMenu* menu, QAction* insertBeforeAction)
{
  if(!insertBeforeAction) {
    recentFilesSeparator = menu->addSeparator();
  }

  for (unsigned int i = 0; i < maxNumberOfRecentFiles(); ++i) {
    recentFileActs[i] = new QAction(this);
    recentFileActs[i]->setVisible(false);
    connect(recentFileActs[i], SIGNAL(triggered()),
            this, SLOT(openRecentFile_aux()));
    if(insertBeforeAction)
      menu->insertAction(insertBeforeAction, recentFileActs[i]);
    else
      menu->addAction(recentFileActs[i]);
  }

  if(insertBeforeAction) {
    recentFilesSeparator = menu->insertSeparator(insertBeforeAction);
  }

  recentFilesSeparator->setVisible(false);

  updateRecentFileActions();
}

void 
DemosMainWindow::updateRecentFileActions()
{
  QSettings settings;
  QStringList files = settings.value("recentFileList").toStringList();

  int numRecentFiles = qMin(files.size(), (int)this->maxNumberOfRecentFiles());
  
  for (int i = 0; i < numRecentFiles; ++i) {
    QString strippedName = QFileInfo(files[i]).fileName();
    QString text = tr("&%1 %2").arg(i).arg(strippedName);
    recentFileActs[i]->setText(text);
    recentFileActs[i]->setData(files[i]);
    recentFileActs[i]->setVisible(true);
  }
  for (unsigned int j = numRecentFiles; j < maxNumberOfRecentFiles(); ++j)
    recentFileActs[j]->setVisible(false);
  
  recentFilesSeparator->setVisible(numRecentFiles > 0);
}

} // namespace Qt
} // namespace CGAL

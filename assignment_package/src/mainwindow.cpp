#include "mainwindow.h"
#include <ui_mainwindow.h>
#include <QFileDialog>

#include "cameracontrolshelp.h"

#include <iostream>
#include <QDoubleSpinBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mygl->setFocus();
    ui->ModeTabWidget->setCurrentIndex(0);
    ui->SkeletonEditTab->setCurrentIndex(0);

    ui->vertsListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->halfEdgesListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->facesListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);

    m_VertPosEditor.Init(ui->vertPosXSpinBox, ui->vertPosYSpinBox, ui->vertPosZSpinBox);
    m_FaceColorEditor.Init(ui->faceRedSpinBox, ui->faceGreenSpinBox, ui->faceBlueSpinBox);
    m_JointLocationEditor.Init(ui->jointPosXSpinBox, ui->jointPosYSpinBox, ui->jointPosZSpinBox);
    m_JointOrientationEditor.Init(ui->jointRotXSpinBox, ui->jointRotYSpinBox, ui->jointRotZSpinBox);

    m_VertexJointWeightEditor.Init({ui->Joint0_Name, ui->Joint1_Name, ui->Joint2_Name, ui->Joint3_Name},
                                    {ui->Joint0_weight, ui->Joint1_weight, ui->Joint2_weight, ui->Joint3_weight});
    ui->mygl->ui = ui;
    ui->mygl->m_VertPosEditor = &m_VertPosEditor;
    ui->mygl->m_FaceColorEditor = &m_FaceColorEditor;
    ui->mygl->m_JointLocationEditor = &m_JointLocationEditor;
    ui->mygl->m_JointOrientationEditor = &m_JointOrientationEditor;
    ui->mygl->m_VertexJointWeightEditor = &m_VertexJointWeightEditor;
    connect(ui->mygl,
            SIGNAL(sig_SendSkeletonTreeRoot(QTreeWidgetItem*)),
            this,
            SLOT(slot_addRootToTreeWidget(QTreeWidgetItem*)));

    connect(ui->mygl,
            SIGNAL(sig_AddListItems(QListWidgetItem**,
                                const unsigned int&,
                                const std::function<QListWidget*(Ui::MainWindow*)>&)),
            this,
            SLOT(slot_AddListItems(QListWidgetItem**,
                                   const unsigned int&,
                                   const std::function<QListWidget*(Ui::MainWindow*)>&)));

    connect(ui->mygl,
            SIGNAL(sig_SetListSelection(QListWidgetItem*,
                                        QItemSelectionModel::SelectionFlags,
                                        const std::function<QListWidget*(Ui::MainWindow*)>&)),
            this,
            SLOT(slot_SetListSelection(QListWidgetItem*,
                                       QItemSelectionModel::SelectionFlags,
                                       const std::function<QListWidget*(Ui::MainWindow*)>&)));

    connect(ui->ModeTabWidget,
            SIGNAL(currentChanged(int)),
            ui->mygl,
            SLOT(slot_ModeChanged(int)));

    connect(ui->BN_Skinning,
            SIGNAL(clicked()),
            ui->mygl,
            SLOT(slot_Skinning()));

    //initially disable spin boxes
    m_VertPosEditor.SetEnabled(false);
    m_FaceColorEditor.SetEnabled(false);
    m_JointLocationEditor.SetEnabled(false);
    m_JointOrientationEditor.SetEnabled(false);
    m_VertexJointWeightEditor.SetDisabled(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_actionCamera_Controls_triggered()
{
    CameraControlsHelp* c = new CameraControlsHelp();
    c->show();
}

void MainWindow::on_actionLoad_OBJ_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    tr("Load OBJ"),
                                                    "../obj_files",
                                                    tr("OBJ Files (*.obj)"));
    ui->mygl->UpdateMesh(filePath);
}

void MainWindow::on_actionLoad_Skeleton_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                                                    tr("Load Skeleton"),
                                                    "../jsons",
                                                    tr("JSON Files (*.json)"));
    ui->mygl->LoadSkeleton(filePath);
}

void MainWindow::slot_AddListItems(QListWidgetItem** first,
                                   const unsigned int& count,
                                   const std::function<QListWidget*(Ui::MainWindow*)>& f)
{
    QListWidget* list = f(ui);
    list->clearFocus(); // avoid the listwidget sending the unexpected signal "currentItemChanged"
    for(size_t i = 0; i < count; ++i)
    {
        list->addItem(*(first + i));
    }
}

void MainWindow::slot_SetListSelection(QListWidgetItem* item,
                                       QItemSelectionModel::SelectionFlags flag,
                                       const std::function<QListWidget*(Ui::MainWindow*)>& f)
{
    QListWidget* list = f(ui);
    list->setCurrentItem(item, flag);
}

void MainWindow::slot_addRootToTreeWidget(QTreeWidgetItem* i)
{
    ui->SkeletonTree->clear();
    ui->SkeletonTree->addTopLevelItem(i);
}

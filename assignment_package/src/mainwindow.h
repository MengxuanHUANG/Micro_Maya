#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include <memory>
#include <functional>
#include <QListWidget>
#include <QTreeWidget>
#include "guivec3editor.h"
#include "vertexjointweighteditor.h"

#include <glm/glm.hpp>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionQuit_triggered();

    void on_actionCamera_Controls_triggered();

    void on_actionLoad_OBJ_triggered();
    void on_actionLoad_Skeleton_triggered();

    void slot_AddListItems(QListWidgetItem** first,
                           const unsigned int& count,
                           const std::function<QListWidget*(Ui::MainWindow*)>& f);

    void slot_SetListSelection(QListWidgetItem* item,
                               QItemSelectionModel::SelectionFlags flag,
                               const std::function<QListWidget*(Ui::MainWindow*)>& f);

    void slot_addRootToTreeWidget(QTreeWidgetItem*);
private:
    Ui::MainWindow *ui;

    GUIVec3Editor m_VertPosEditor;
    GUIVec3Editor m_FaceColorEditor;

    GUIVec3Editor m_JointLocationEditor;
    GUIVec3Editor m_JointOrientationEditor;

    VertexJointWeightEditor m_VertexJointWeightEditor;
};

#endif // MAINWINDOW_H

#include "meshmode.h"

#include <ui_mainwindow.h>
#include <QApplication>
#include <QKeyEvent>
#include <QListWidgetItem>

#include "camera.h"
#include "guivec3editor.h"
#include "mygl.h"
#include "guivec3editor.h"

#include "mesh.h"
#include "halfedge.h"
#include "vertex.h"
#include "face.h"
#include "raycaster.h"
#include "scene/wireframesphere.h"
#include "scene/skeleton.h"
#include "scene/joint.h"

MeshMode::MeshMode(MyGL* mygl, Mode* parent)
    :Mode(mygl, parent),
     m_VertexDisplay(std::make_unique<VertexDisplay>(mygl)),
     m_HalfEdgeDisplay(std::make_unique<HalfEdgeDisplay>(mygl)),
     m_FaceDisplay(std::make_unique<FaceDisplay>(mygl))
{}

void MeshMode::Load()
{
    connect(m_VertexList,
            SIGNAL(itemSelectionChanged()),
            this,
            SLOT(slot_vertexSelectionChanged()));

    connect(m_HalfEdgeList,
            SIGNAL(itemSelectionChanged()),
            this,
            SLOT(slot_halfEdgeSelectionChanged()));

    connect(m_FaceList,
            SIGNAL(itemSelectionChanged()),
            this,
            SLOT(slot_faceSelectionChanged()));

    connect(BN_AddVertex,
            SIGNAL(clicked()),
            this,
            SLOT(slot_AddVertex()));

    connect(BN_TriangulateFace,
            SIGNAL(clicked()),
            this,
            SLOT(slot_TriangulateFace()));

    connect(BN_ExtrudeFace,
            SIGNAL(clicked()),
            this,
            SLOT(slot_ExtrudeFace()));

    connect(BN_Subdivision,
            SIGNAL(clicked()),
            this,
            SLOT(slot_MeshSubdivision()));

    connect(BN_MarkSharp,
            SIGNAL(clicked()),
            this,
            SLOT(slot_MarkSharp()));

    connect(BN_Bevel,
            SIGNAL(clicked()),
            this,
            SLOT(slot_Bevel()));

    connect(BN_InsertEdgeLoop,
            SIGNAL(clicked()),
            this,
            SLOT(slot_InsertEdgeLoop()));

    // position connection
    connect(m_VertexPosEditor,
            SIGNAL(sig_Vec3Changed(const glm::vec3&)),
            this,
            SLOT(slot_VertPosChanged(const glm::vec3&)));

    // color connection
    connect(m_FaceColEditor,
            SIGNAL(sig_Vec3Changed(const glm::vec3&)),
            this,
            SLOT(slot_FaceColorChanged(const glm::vec3&)));
}

void MeshMode::UnLoad()
{
    ClearSelection();

    disconnect(m_VertexList,
               SIGNAL(itemSelectionChanged()),
               this,
               SLOT(slot_vertexSelectionChanged()));

    disconnect(m_HalfEdgeList,
               SIGNAL(itemSelectionChanged()),
               this,
               SLOT(slot_halfEdgeSelectionChanged()));

    disconnect(m_FaceList,
               SIGNAL(itemSelectionChanged()),
               this,
               SLOT(slot_faceSelectionChanged()));

    disconnect(BN_AddVertex,
               SIGNAL(clicked()),
               this,
               SLOT(slot_AddVertex()));

    disconnect(BN_TriangulateFace,
               SIGNAL(clicked()),
               this,
               SLOT(slot_TriangulateFace()));

    disconnect(BN_ExtrudeFace,
               SIGNAL(clicked()),
               this,
               SLOT(slot_ExtrudeFace()));

    disconnect(BN_Subdivision,
               SIGNAL(clicked()),
               this,
               SLOT(slot_MeshSubdivision()));

    disconnect(BN_MarkSharp,
               SIGNAL(clicked()),
               this,
               SLOT(slot_MarkSharp()));

    disconnect(BN_Bevel,
               SIGNAL(clicked()),
               this,
               SLOT(slot_Bevel()));

    disconnect(BN_InsertEdgeLoop,
               SIGNAL(clicked()),
               this,
               SLOT(slot_InsertEdgeLoop()));

    // position connection
    disconnect(m_VertexPosEditor,
               SIGNAL(sig_Vec3Changed(const glm::vec3&)),
               this,
               SLOT(slot_VertPosChanged(const glm::vec3&)));

    // color connection
    disconnect(m_FaceColEditor,
               SIGNAL(sig_Vec3Changed(const glm::vec3&)),
               this,
               SLOT(slot_FaceColorChanged(const glm::vec3&)));
}

void MeshMode::ClearSelection()
{
    m_VertexList->clearSelection();
    m_HalfEdgeList->clearSelection();
    m_FaceList->clearSelection();

    m_SharpEdges.clear();
    m_sharpeVertices.clear();
}

void MeshMode::Init()
{
    m_VertexDisplay->Init();
    m_HalfEdgeDisplay->Init();
    m_FaceDisplay->Init();
}

void MeshMode::Draw()
{
    m_progLambert->setViewProjMatrix(m_Camera->getViewProj());
    m_progLambert->setCamPos(m_Camera->eye);

    m_progLambert->setModelMatrix(m_Mesh->GetModelMatrix());
    m_progLambert->draw(*static_cast<Drawable*>(m_Mesh));
}

void MeshMode::DrawOnTop()
{
    m_progFlat->setViewProjMatrix(m_Camera->getViewProj());
    m_progFlat->setModelMatrix(glm::mat4(1.f));

    m_progFlat->setModelMatrix(m_Mesh->GetModelMatrix());
    m_progFlat->draw(*static_cast<Drawable*>(m_FaceDisplay.get()));
    m_progFlat->draw(*static_cast<Drawable*>(m_HalfEdgeDisplay.get()));
    m_progFlat->draw(*static_cast<Drawable*>(m_VertexDisplay.get()));
}

void MeshMode::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_N && m_HalfEdgeDisplay->GetCurrentSelectedHalfEdge() != nullptr)
    {
        QItemSelectionModel::SelectionFlags flag = QApplication::queryKeyboardModifiers().testFlag(Qt::ShiftModifier) ?
                                                    QItemSelectionModel::Select : QItemSelectionModel::QItemSelectionModel::ClearAndSelect;

        m_MyGL->sig_SetListSelection(reinterpret_cast<QListWidgetItem*>(m_HalfEdgeDisplay->GetCurrentSelectedHalfEdge()->GetNext()),
                                        flag,
                                        [](Ui::MainWindow* ui) { return ui->halfEdgesListWidget;});
    }
    else if (e->key() == Qt::Key_M && m_HalfEdgeDisplay->GetCurrentSelectedHalfEdge() != nullptr)
    {
        QItemSelectionModel::SelectionFlags flag = QApplication::queryKeyboardModifiers().testFlag(Qt::ShiftModifier) ?
                                                    QItemSelectionModel::Select : QItemSelectionModel::QItemSelectionModel::ClearAndSelect;

        m_MyGL->sig_SetListSelection(reinterpret_cast<QListWidgetItem*>(m_HalfEdgeDisplay->GetCurrentSelectedHalfEdge()->GetSymmetric()),
                                        flag,
                                        [](Ui::MainWindow* ui) { return ui->halfEdgesListWidget;});
    }
    else if (e->key() == Qt::Key_F && m_HalfEdgeDisplay->GetCurrentSelectedHalfEdge() != nullptr)
    {
        QItemSelectionModel::SelectionFlags flag = QApplication::queryKeyboardModifiers().testFlag(Qt::ShiftModifier) ?
                                                    QItemSelectionModel::Select : QItemSelectionModel::QItemSelectionModel::ClearAndSelect;

        m_MyGL->sig_SetListSelection(reinterpret_cast<QListWidgetItem*>(m_HalfEdgeDisplay->GetCurrentSelectedHalfEdge()->GetFace()),
                                        flag,
                                        [](Ui::MainWindow* ui) { return ui->facesListWidget;});
    }
    else if (e->key() == Qt::Key_V && m_HalfEdgeDisplay->GetCurrentSelectedHalfEdge() != nullptr)
    {
        QItemSelectionModel::SelectionFlags flag = QApplication::queryKeyboardModifiers().testFlag(Qt::ShiftModifier) ?
                                                    QItemSelectionModel::Select : QItemSelectionModel::QItemSelectionModel::ClearAndSelect;

        m_MyGL->sig_SetListSelection(reinterpret_cast<QListWidgetItem*>(m_HalfEdgeDisplay->GetCurrentSelectedHalfEdge()->GetVertex()),
                                        flag,
                                        [](Ui::MainWindow* ui) { return ui->vertsListWidget;});
    }
    else if (e->keyCombination() == QKeyCombination(Qt::SHIFT, Qt::Key_H) && m_FaceDisplay->GetCurrentSelectedFace() != nullptr)
    {
        m_MyGL->sig_SetListSelection(reinterpret_cast<QListWidgetItem*>(m_FaceDisplay->GetCurrentSelectedFace()->GetHalfEdge()),
                                        QItemSelectionModel::ClearAndSelect,
                                        [](Ui::MainWindow* ui) { return ui->halfEdgesListWidget;});
    }
    else if (e->key() == Qt::Key_H && m_VertexDisplay->GetCurrentSelectedVertex() != nullptr)
    {
        m_MyGL->sig_SetListSelection(reinterpret_cast<QListWidgetItem*>(m_VertexDisplay->GetCurrentSelectedVertex()->GetHalfEdge()),
                                        QItemSelectionModel::ClearAndSelect,
                                        [](Ui::MainWindow* ui) { return ui->halfEdgesListWidget;});
    }

    m_MyGL->update();
}

void MeshMode::MousePressEvent(QMouseEvent *event)
{
    const QPoint& pos = event->pos();
    std::optional selected = SelectAt(pos.x(), pos.y());
    bool shift = QApplication::queryKeyboardModifiers().testFlag(Qt::ShiftModifier);
    QItemSelectionModel::SelectionFlags flag =  shift ?
                                                QItemSelectionModel::Select : QItemSelectionModel::QItemSelectionModel::ClearAndSelect;
    if(selected.has_value())
    {
        // draw components based on type
        switch(selected->type)
        {
        case ElementType::VERTEX:
        {
            Vertex* v = reinterpret_cast<Vertex*>(selected->item);
            if(shift && m_VertexDisplay->IsSelected(v))
            {
                m_MyGL->sig_SetListSelection(selected->item,
                                                QItemSelectionModel::Deselect,
                                                [](Ui::MainWindow* ui) { return ui->vertsListWidget;});
            }
            else
            {
                m_MyGL->sig_SetListSelection(selected->item,
                                                flag,
                                                [](Ui::MainWindow* ui) { return ui->vertsListWidget;});
            }
            break;
        }
        case ElementType::HALFEDGE:
        {
            HalfEdge* e = reinterpret_cast<HalfEdge*>(selected->item);
            if(shift && m_HalfEdgeDisplay->IsSelected(e))
            {
                m_MyGL->sig_SetListSelection(selected->item,
                                                QItemSelectionModel::Deselect,
                                                [](Ui::MainWindow* ui) { return ui->halfEdgesListWidget;});
            }
            else
            {
                m_MyGL->sig_SetListSelection(selected->item,
                                                flag,
                                                [](Ui::MainWindow* ui) { return ui->halfEdgesListWidget;});
            }
            break;
        }
        case ElementType::FACE:
        {
            Face* f = reinterpret_cast<Face*>(selected->item);
            if(shift && m_FaceDisplay->IsSelected(f))
            {
                m_MyGL->sig_SetListSelection(selected->item,
                                                QItemSelectionModel::Deselect,
                                                [](Ui::MainWindow* ui) { return ui->facesListWidget;});
            }
            else
            {
                m_MyGL->sig_SetListSelection(selected->item,
                                                flag,
                                                [](Ui::MainWindow* ui) { return ui->facesListWidget;});
            }
            break;
        }
        default:
            break;
        }
    }
    else if(!QApplication::queryKeyboardModifiers().testFlag(Qt::AltModifier))
    {
        m_MyGL->sig_SetListSelection(nullptr,
                                        QItemSelectionModel::Clear,
                                        [](Ui::MainWindow* ui) { return ui->vertsListWidget;});
        m_MyGL->sig_SetListSelection(nullptr,
                                        QItemSelectionModel::Clear,
                                        [](Ui::MainWindow* ui) { return ui->halfEdgesListWidget;});
        m_MyGL->sig_SetListSelection(nullptr,
                                        QItemSelectionModel::Clear,
                                        [](Ui::MainWindow* ui) { return ui->facesListWidget;});
    }

    m_MyGL->update();
}

void MeshMode::MouseDoubleClickEvent(QMouseEvent *event)
{
    const QPoint& pos = event->pos();
    std::optional selected = SelectAt(pos.x(), pos.y());

    if(QApplication::queryKeyboardModifiers().testFlag(Qt::ShiftModifier) &&
       selected.has_value())
    {
        // draw components based on type
        switch(selected->type)
        {
        case ElementType::HALFEDGE:
        {
            HalfEdge* previousSelected = m_HalfEdgeDisplay->GetPreviousSelectedHalfEdge();
            if(previousSelected != nullptr)
            {
                m_HalfEdgeDisplay->GetEdgeLoop().clear();
                bool found = HalfEdge::TryFindEdgeLoop(previousSelected,
                                                       reinterpret_cast<HalfEdge*>(selected->item),
                                                       m_HalfEdgeDisplay->GetEdgeLoop().m_EdgeLoop,
                                                       m_HalfEdgeDisplay->GetEdgeLoop().m_Pattern);
                for(HalfEdge* e : m_HalfEdgeDisplay->GetEdgeLoop().m_EdgeLoop)
                {
                    m_MyGL->sig_SetListSelection(e,
                                                    QItemSelectionModel::Select,
                                                    [](Ui::MainWindow* ui) { return ui->halfEdgesListWidget;});
                }
                if(!found || m_HalfEdgeDisplay->GetEdgeLoop().m_Pattern.empty())
                {
                    m_HalfEdgeDisplay->GetEdgeLoop().clear();
                }
            }
            break;
        }
        default:
            break;
        }
    }
    m_MyGL->update();
}

void MeshMode::MarkFaceSharp(const QList<QListWidgetItem*>& list)
{
    for(QListWidgetItem* item : list)
    {
        Face* f = reinterpret_cast<Face*>(item);
        HalfEdge* e = f->GetHalfEdge();
        do
        {
            m_sharpeVertices.emplace(e->GetVertex()->GetIndex());
            m_sharpeVertices.emplace(e->GetSymmetric()->GetVertex()->GetIndex());

            m_SharpEdges.emplace(e->GetIndex());
            m_SharpEdges.emplace(e->GetSymmetric()->GetIndex());
            e = e->GetNext();
        }
        while(e != f->GetHalfEdge());
    }
}

void MeshMode::MeshMode::UpdateDrawable()
{
    m_Mesh->create();
    m_VertexDisplay->create();
    m_HalfEdgeDisplay->create();
    m_FaceDisplay->create();

    m_MyGL->update();
}

std::optional<RayCastResult> MeshMode::SelectAt(const unsigned int& x, const unsigned int& y)
{
    glm::vec3 posInWorld = m_MyGL->GetPxielInWorld(x, y, m_Camera->far_clip);
    glm::vec3 rayDir = glm::normalize(posInWorld - m_Camera->eye);
    return RayCaster::CastRayToMesh(m_Camera->eye, rayDir, m_Camera, m_Mesh);
}

void MeshMode::slot_VertPosChanged(const glm::vec3& pos)
{
    m_VertexDisplay->GetCurrentSelectedVertex()->SetPosition(pos);
    UpdateDrawable();
}

void MeshMode::slot_FaceColorChanged(const glm::vec3& color)
{
    m_FaceDisplay->GetCurrentSelectedFace()->SetColor(color);
    UpdateDrawable();
}

void MeshMode::slot_MarkSharp()
{
    MarkVertexSharp(m_VertexList->selectedItems());
    MarkHalfEdgeSharp(m_HalfEdgeList->selectedItems());
    MarkFaceSharp(m_FaceList->selectedItems());
}

void MeshMode::slot_vertexSelectionChanged()
{
    const QList<QListWidgetItem*>& list = m_VertexList->selectedItems();
    m_VertexDisplay->ClearSelection();
    for(QListWidgetItem* item : list)
    {
        Vertex* v = reinterpret_cast<Vertex*>(item);
        m_VertexDisplay->SelectVertex(v);
    }

    // disable edit when select more than one vertices
    if(list.size() == 1)
    {
        m_VertexPosEditor->SetEnabled(true);
        m_VertexPosEditor->SetValue(m_VertexDisplay->GetCurrentSelectedVertex()->GetPosition(), true);
    }
    else
    {
        m_VertexPosEditor->SetEnabled(false);
        m_VertexPosEditor->SetValue(glm::vec3(0), true);
    }
    m_VertexDisplay->create();
    m_MyGL->update();
}

void MeshMode::slot_halfEdgeSelectionChanged()
{
    const QList<QListWidgetItem*>& list = m_HalfEdgeList->selectedItems();
    m_HalfEdgeDisplay->ClearSelection();
    for(QListWidgetItem* item : list)
    {
        HalfEdge* e = reinterpret_cast<HalfEdge*>(item);
        m_HalfEdgeDisplay->SelectHalfEdge(e);
    }
    if(list.size() == 0)
    {
        m_HalfEdgeDisplay->GetEdgeLoop().clear();
    }
    m_HalfEdgeDisplay->create();
    m_MyGL->update();
}

void MeshMode::slot_faceSelectionChanged()
{
    const QList<QListWidgetItem*>& list = m_FaceList->selectedItems();
    m_FaceDisplay->ClearSelection();
    for(QListWidgetItem* item : list)
    {
        Face* f = reinterpret_cast<Face*>(item);
        m_FaceDisplay->SelectFace(f);
    }
    // disable edit when select more than one vertices
    if(list.size() == 1)
    {
        m_FaceColEditor->SetEnabled(true);
        m_FaceColEditor->SetValue(m_FaceDisplay->GetCurrentSelectedFace()->GetColor(), true);
    }
    else
    {
        m_FaceColEditor->SetEnabled(false);
        m_FaceColEditor->SetValue(glm::vec3(0), true);
    }
    m_FaceDisplay->create();
    m_MyGL->update();
}

void MeshMode::slot_AddVertex()
{
    std::vector<Vertex*> newVertices;
    if(!m_HalfEdgeDisplay->GetSelectedHalfEdges().empty())
    {
        size_t v_count = m_Mesh->m_Vertices.size();
        size_t e_count = m_Mesh->m_HalfEdges.size();
        newVertices.reserve(m_HalfEdgeDisplay->GetSelectedHalfEdges().size());
        for(HalfEdge* e : m_HalfEdgeDisplay->GetSelectedHalfEdges())
        {
            // divide edge
            Vertex* newVertex = m_Mesh->DivideEdge(e, 0.5f);
            newVertices.emplace_back(newVertex);
        }
        size_t v_added = m_Mesh->m_Vertices.size() - v_count;
        size_t e_added = m_Mesh->m_HalfEdges.size() - e_count;
        // update GUI
        m_MyGL->sig_AddListItems(reinterpret_cast<QListWidgetItem**>(&m_Mesh->m_Vertices[m_Mesh->m_Vertices.size() - v_added]),
                                    v_added,
                                    [](Ui::MainWindow* ui) { return ui->vertsListWidget;});
        m_MyGL->sig_AddListItems(reinterpret_cast<QListWidgetItem**>(&m_Mesh->m_HalfEdges[m_Mesh->m_HalfEdges.size() - e_added]),
                                    e_added,
                                    [](Ui::MainWindow* ui) { return ui->halfEdgesListWidget;});
        for(Vertex* v : newVertices)
        {
            m_MyGL->sig_SetListSelection(v,
                                            QItemSelectionModel::Select,
                                            [](Ui::MainWindow* ui) { return ui->vertsListWidget;});
        }
        UpdateDrawable();
    }
}

void MeshMode::slot_TriangulateFace()
{
    if(!m_FaceDisplay->GetSelectedFaces().empty())
    {
        size_t e_count = m_Mesh->m_HalfEdges.size();
        size_t f_count = m_Mesh->m_Faces.size();
        for(Face* f : m_FaceDisplay->GetSelectedFaces())
        {
            m_Mesh->TriangulateFace(f);
        }
        size_t e_added = m_Mesh->m_HalfEdges.size() - e_count;
        size_t f_added = m_Mesh->m_Faces.size() - f_count;

        // update GUI
        m_MyGL->sig_AddListItems(reinterpret_cast<QListWidgetItem**>(&m_Mesh->m_HalfEdges[m_Mesh->m_HalfEdges.size() - e_added]),
                                    e_added,
                                    [](Ui::MainWindow* ui) { return ui->halfEdgesListWidget;});
        m_MyGL->sig_AddListItems(reinterpret_cast<QListWidgetItem**>(&m_Mesh->m_Faces[m_Mesh->m_Faces.size() - f_added]),
                                    f_added,
                                    [](Ui::MainWindow* ui) { return ui->facesListWidget;});

        UpdateDrawable();
    }
}

void MeshMode::slot_ExtrudeFace()
{
    if(!m_FaceDisplay->GetSelectedFaces().empty())
    {
        size_t v_count = m_Mesh->m_Vertices.size();
        size_t e_count = m_Mesh->m_HalfEdges.size();
        size_t f_count = m_Mesh->m_Faces.size();

        for(Face* f : m_FaceDisplay->GetSelectedFaces())
        {
            m_Mesh->ExtrudeFace(f, 1);
        }

        size_t v_added = m_Mesh->m_Vertices.size() - v_count;
        size_t e_added = m_Mesh->m_HalfEdges.size() - e_count;
        size_t f_added = m_Mesh->m_Faces.size() - f_count;

        // update GUI
        m_MyGL->sig_AddListItems(reinterpret_cast<QListWidgetItem**>(&m_Mesh->m_Vertices[m_Mesh->m_Vertices.size() - v_added]),
                                    v_added,
                                    [](Ui::MainWindow* ui) { return ui->vertsListWidget;});

        m_MyGL->sig_AddListItems(reinterpret_cast<QListWidgetItem**>(&m_Mesh->m_HalfEdges[m_Mesh->m_HalfEdges.size() - e_added]),
                                    e_added,
                                    [](Ui::MainWindow* ui) { return ui->halfEdgesListWidget;});
        m_MyGL->sig_AddListItems(reinterpret_cast<QListWidgetItem**>(&m_Mesh->m_Faces[m_Mesh->m_Faces.size() - f_added]),
                                    f_added,
                                    [](Ui::MainWindow* ui) { return ui->facesListWidget;});

        UpdateDrawable();
    }
}

void MeshMode::slot_MeshSubdivision()
{
    size_t v_count = m_Mesh->m_Vertices.size();
    size_t e_count = m_Mesh->m_HalfEdges.size();
    size_t f_count = m_Mesh->m_Faces.size();

    m_Mesh->Subdivision(m_SharpEdges, m_sharpeVertices);

    size_t v_added = m_Mesh->m_Vertices.size() - v_count;
    size_t e_added = m_Mesh->m_HalfEdges.size() - e_count;
    size_t f_added = m_Mesh->m_Faces.size() - f_count;

    // update GUI
    m_MyGL->sig_AddListItems(reinterpret_cast<QListWidgetItem**>(&m_Mesh->m_Vertices[m_Mesh->m_Vertices.size() - v_added]),
                                v_added,
                                [](Ui::MainWindow* ui) { return ui->vertsListWidget;});

    m_MyGL->sig_AddListItems(reinterpret_cast<QListWidgetItem**>(&m_Mesh->m_HalfEdges[m_Mesh->m_HalfEdges.size() - e_added]),
                                e_added,
                                [](Ui::MainWindow* ui) { return ui->halfEdgesListWidget;});
    m_MyGL->sig_AddListItems(reinterpret_cast<QListWidgetItem**>(&m_Mesh->m_Faces[m_Mesh->m_Faces.size() - f_added]),
                                f_added,
                                [](Ui::MainWindow* ui) { return ui->facesListWidget;});

    UpdateDrawable();
}

void MeshMode::slot_Bevel()
{
    if(!m_FaceDisplay->GetSelectedFaces().empty())
    {
        size_t v_count = m_Mesh->m_Vertices.size();
        size_t e_count = m_Mesh->m_HalfEdges.size();
        size_t f_count = m_Mesh->m_Faces.size();

        for(Face* f : m_FaceDisplay->GetSelectedFaces())
        {
            m_Mesh->BevelFace(f, 4, 0.5f, 1.0f);
        }

        size_t v_added = m_Mesh->m_Vertices.size() - v_count;
        size_t e_added = m_Mesh->m_HalfEdges.size() - e_count;
        size_t f_added = m_Mesh->m_Faces.size() - f_count;

        // update GUI
        m_MyGL->sig_AddListItems(reinterpret_cast<QListWidgetItem**>(&m_Mesh->m_Vertices[m_Mesh->m_Vertices.size() - v_added]),
                                    v_added,
                                    [](Ui::MainWindow* ui) { return ui->vertsListWidget;});

        m_MyGL->sig_AddListItems(reinterpret_cast<QListWidgetItem**>(&m_Mesh->m_HalfEdges[m_Mesh->m_HalfEdges.size() - e_added]),
                                    e_added,
                                    [](Ui::MainWindow* ui) { return ui->halfEdgesListWidget;});
        m_MyGL->sig_AddListItems(reinterpret_cast<QListWidgetItem**>(&m_Mesh->m_Faces[m_Mesh->m_Faces.size() - f_added]),
                                    f_added,
                                    [](Ui::MainWindow* ui) { return ui->facesListWidget;});

        UpdateDrawable();
    }
}

void MeshMode::slot_InsertEdgeLoop()
{
    if(m_HalfEdgeDisplay->GetEdgeLoop().m_EdgeLoop.size() > 0)
    {
        size_t v_count = m_Mesh->m_Vertices.size();
        size_t e_count = m_Mesh->m_HalfEdges.size();
        size_t f_count = m_Mesh->m_Faces.size();

        m_Mesh->InsertEdgeLoop(m_HalfEdgeDisplay->GetEdgeLoop().m_EdgeLoop, 0.5f);

        size_t v_added = m_Mesh->m_Vertices.size() - v_count;
        size_t e_added = m_Mesh->m_HalfEdges.size() - e_count;
        size_t f_added = m_Mesh->m_Faces.size() - f_count;

        // update GUI
        m_MyGL->sig_AddListItems(reinterpret_cast<QListWidgetItem**>(&m_Mesh->m_Vertices[m_Mesh->m_Vertices.size() - v_added]),
                                    v_added,
                                    [](Ui::MainWindow* ui) { return ui->vertsListWidget;});

        m_MyGL->sig_AddListItems(reinterpret_cast<QListWidgetItem**>(&m_Mesh->m_HalfEdges[m_Mesh->m_HalfEdges.size() - e_added]),
                                    e_added,
                                    [](Ui::MainWindow* ui) { return ui->halfEdgesListWidget;});
        m_MyGL->sig_AddListItems(reinterpret_cast<QListWidgetItem**>(&m_Mesh->m_Faces[m_Mesh->m_Faces.size() - f_added]),
                                    f_added,
                                    [](Ui::MainWindow* ui) { return ui->facesListWidget;});

        UpdateDrawable();
    }
}

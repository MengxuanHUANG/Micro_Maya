#ifndef MESHMODE_H
#define MESHMODE_H

#include "mode.h"
#include <unordered_set>
#include <utils.h>
#include <memory>
#include <functional>
#include <QListWidget>
#include <glm/glm.hpp>

#include "scene/vertexdisplay.h"
#include "scene/halfedgedisplay.h"
#include "scene/facedisplay.h"
#include "halfedge.h"
#include "vertex.h"

class MyGL;

class Camera;
class Mesh;
class Skeleton;
class WireframeSphere;

class ShaderProgram;
class GUIVec3Editor;
class QPushButton;

struct RayCastResult;

class MeshMode : public Mode
{
    Q_OBJECT
public:
    MeshMode(MyGL* mygl, Mode* parent);

    virtual void Load() override;
    virtual void UnLoad() override;
    virtual void ClearSelection() override;

    virtual void Init() override;
    // define how to draw
    virtual void Draw() override;
    virtual void DrawOnTop() override;

    // Input events
    virtual void keyPressEvent(QKeyEvent *e) override;
    virtual void MousePressEvent(QMouseEvent *event) override;
    virtual void MouseDoubleClickEvent(QMouseEvent *event) override;

signals:

private slots:
    void slot_VertPosChanged(const glm::vec3&);
    void slot_FaceColorChanged(const glm::vec3&);

    void slot_MarkSharp();
    void slot_vertexSelectionChanged();
    void slot_halfEdgeSelectionChanged();
    void slot_faceSelectionChanged();

    void slot_AddVertex();
    void slot_TriangulateFace();
    void slot_ExtrudeFace();
    void slot_MeshSubdivision();
    void slot_Bevel();
    void slot_InsertEdgeLoop();

protected:
    void UpdateDrawable();
    std::optional<RayCastResult> SelectAt(const unsigned int&, const unsigned int&);

    // mark components as sharp
    void MarkFaceSharp(const QList<QListWidgetItem*>& list);
    inline void MarkHalfEdgeSharp(const QList<QListWidgetItem*>& list)
    {
        for(QListWidgetItem* item : list)
        {
            HalfEdge* e = reinterpret_cast<HalfEdge*>(item);
            m_SharpEdges.emplace(e->GetIndex());
            m_SharpEdges.emplace(e->GetSymmetric()->GetIndex());
        }
    }
    inline void MarkVertexSharp(const QList<QListWidgetItem*>& list)
    {
        for(QListWidgetItem* item : list)
        {
            Vertex* v = reinterpret_cast<Vertex*>(item);
            m_sharpeVertices.emplace(v->GetIndex());
        }
    }

public:
    Camera* m_Camera;
    Mesh* m_Mesh;

    std::unique_ptr<VertexDisplay> m_VertexDisplay;
    std::unique_ptr<HalfEdgeDisplay> m_HalfEdgeDisplay;
    std::unique_ptr<FaceDisplay> m_FaceDisplay;

    ShaderProgram* m_progLambert;
    ShaderProgram* m_progFlat;

    // UI
    QListWidget* m_VertexList;
    QListWidget* m_HalfEdgeList;
    QListWidget* m_FaceList;

    GUIVec3Editor* m_VertexPosEditor;
    GUIVec3Editor* m_FaceColEditor;

    QPushButton* BN_AddVertex;
    QPushButton* BN_TriangulateFace;
    QPushButton* BN_ExtrudeFace;
    QPushButton* BN_Bevel;
    QPushButton* BN_InsertEdgeLoop;
    QPushButton* BN_Subdivision;
    QPushButton* BN_MarkSharp;

    std::unordered_set<unsigned int> m_SharpEdges;
    std::unordered_set<unsigned int> m_sharpeVertices;
};

#endif // MESHMODE_H

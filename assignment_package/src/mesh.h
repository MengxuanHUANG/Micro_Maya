#ifndef MESH_H
#define MESH_H

#include <drawable.h>

#include <vector>
#include <unordered_set>
#include <memory>

class Vertex;
class HalfEdge;
class Face;

class Mesh : public Drawable
{
public:
    Mesh(OpenGLContext* context);

    void LoadOBJ(const QString& path);
    void ReleaseOBJ();

    virtual void create() override;
    virtual GLenum drawMode() override;

public: // mesh editing operations
    Vertex* DivideEdge(HalfEdge* e, const float& t);
    Vertex* DivideEdge(HalfEdge* e, const glm::vec3& pos);

    void TriangulateFace(Face* f);

    void SeperateFace(Face* f);

    void ExtrudeFace(Face* f, const float& d);

    void BevelFace(Face* f, const unsigned int& segment, const float& width, const float& depth);

    void InsertEdgeLoop(std::vector<HalfEdge*>& edgeLoop,
                        const float& t);

    void Subdivision(std::unordered_set<unsigned int>& sharpEdges,
                     std::unordered_set<unsigned int>& sharpVertices);

public:
    inline glm::mat4& GetModelMatrix() { return m_ModelMatrix; }
    inline void SetModelMatrix(const glm::mat4& model) { m_ModelMatrix = model; }
public:
    std::vector<std::unique_ptr<Face> > m_Faces;
    std::vector<std::unique_ptr<HalfEdge> > m_HalfEdges;
    std::vector<std::unique_ptr<Vertex> > m_Vertices;

protected:
    glm::mat4 m_ModelMatrix;

public:
    static void HelpCreate(Mesh*,
                           std::vector<glm::vec4>&,
                           std::vector<glm::vec4>&,
                           std::vector<glm::vec4>&,
                           std::vector<GLuint>&);
};

#endif // MESH_H

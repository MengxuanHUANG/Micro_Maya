#ifndef SKELETALMESH_H
#define SKELETALMESH_H

#include <drawable.h>
#include <vector>

class Mesh;
class Skeleton;
class Joint;
class Vertex;

class SkeletalMesh : public Drawable
{
public:
    SkeletalMesh(OpenGLContext* context);

    void Bind(Mesh* mesh, Skeleton* skeleton);
    void UnBind();

    inline Skeleton* GetSkeleton() const { return m_Skeleton; }
    inline Mesh* GetMesh() const { return m_Mesh; }
    inline bool IsBinded() const { return m_Binded; }
    inline std::vector<std::vector<float> >& GetVertexWeights() { return m_VertexWeights; }
protected:
    void InitJointWeights();

public:
    // Drawable
    GLuint bufID;
    GLuint bufWeight;

    bool IDBound;
    bool WeightBound;

    void Init();

    void generateID();
    void generateWeight();

    bool bindID();
    bool bindWeight();
public:
    virtual void create() override;
    void CreateWithSelectedJoint(Joint* joint);
    virtual GLenum drawMode() override;

protected:
    bool m_Binded;

    Mesh* m_Mesh;
    Skeleton* m_Skeleton;

    std::vector<std::vector<float> > m_VertexWeights;

public:
    static void HelpCreate(SkeletalMesh*,
                           Joint*,
                           std::vector<glm::vec4>&,
                           std::vector<glm::vec4>&,
                           std::vector<glm::vec4>&,
                           std::vector<glm::vec4>&,
                           std::vector<glm::ivec4>&,
                           std::vector<GLuint>&);

    // assign joint influence to vertices
    static void Skinning(SkeletalMesh* skeletalMesh);
    static void HeatDiffusionSkinning(SkeletalMesh* skeletalMesh);

    // find the cloest vertex of a joint
    // and cache square-distance of all vertices to the joint
    static Vertex* CloestVertex(Joint*,
                                const std::vector<std::unique_ptr<Vertex>>&,
                                std::vector<float>&);

    static void HandleVertex(SkeletalMesh*,
                             Joint*,
                             Vertex*,
                             float,
                             const float&,
                             const float&,
                             const std::vector<float>&,
                             std::vector<bool>&);

    static void GetFirstKWeight(std::vector<float>&,
                                const int&,
                                glm::vec4&,
                                glm::ivec4&);
};

#endif // SKELETALMESH_H

#include "skeletalmesh.h"

#include "mesh.h"
#include "vertex.h"

#include "scene/skeleton.h"
#include "halfedge.h"
#include "joint.h"

#include <limits>
#include <queue>

void SkeletalMesh::Skinning(SkeletalMesh* skeletalMesh)
{
    Mesh* mesh = skeletalMesh->m_Mesh;
    Skeleton* skeleton = skeletalMesh->m_Skeleton;

    for(std::unique_ptr<Vertex>& v : mesh->m_Vertices)
    {
        glm::vec3& pos = v->GetPosition();
        std::vector<float> d2s;
        // find the closet joint
        Joint* closest_joint = nullptr;
        float d2_min = std::numeric_limits<float>::max();
        for(Joint* j : skeleton->GetJoints())
        {
            glm::vec3 jointPos = j->getOverallTransformation().GetTranslation();
            float d2 = glm::length2(pos - jointPos);
            d2s.emplace_back(d2);
            if(d2 < d2_min)
            {
                d2_min = d2;
                closest_joint = j;
            }
        }

        // find the secondly closet joint
        Joint* second_closest_joint = closest_joint->GetParent();
        float second_d2_min = second_closest_joint != nullptr ? d2s[second_closest_joint->GetID()] : std::numeric_limits<float>::max();
        // check whether children is closer
        for(std::unique_ptr<Joint>& child : closest_joint->GetChildren())
        {
            if(d2s[child->GetID()] < second_d2_min)
            {
                second_d2_min = d2s[child->GetID()];
                second_closest_joint = child.get();
            }
        }

        float d1 = glm::sqrt(d2_min);
        float d2 = glm::sqrt(second_d2_min);
        float d_sum = d1 + d2;

        for(int i = 0; i< skeleton->GetJointsCount(); ++i)
        {
            if(skeleton->GetJoints()[i] == closest_joint)
            {
                skeletalMesh->m_VertexWeights[v->GetIndex()][i] = d2 / d_sum;
            }
            else if(skeleton->GetJoints()[i] == second_closest_joint)
            {
                skeletalMesh->m_VertexWeights[v->GetIndex()][i] = d1 / d_sum;
            }
        }
    }
}

void SkeletalMesh::HeatDiffusionSkinning(SkeletalMesh* skeletalMesh)
{
    Mesh* mesh = skeletalMesh->m_Mesh;
    Skeleton* skeleton = skeletalMesh->m_Skeleton;

    for(Joint* joint : skeleton->GetJoints())
    {
        // 1. find the cloest vertex
        std::vector<float> d2s;
        d2s.reserve(mesh->m_Vertices.size());
        Vertex* cloestV = SkeletalMesh::CloestVertex(joint, mesh->m_Vertices, d2s);

        float min_d2 = d2s[cloestV->GetIndex()];
        float min_d = glm::sqrt(min_d2);

        // 2. find all vertices within the sphere (d2 < min_d2 + threshold)

        std::vector<int> unhandledVertices;

        const float threshold = 1.f;
        min_d2 += threshold;
        for(int i = 0; i < d2s.size(); ++i)
        {
            if(d2s[i] < min_d2)
            {
                unhandledVertices.emplace_back(i);
            }
        }

        std::vector<bool> handled;
        handled.resize(mesh->m_Vertices.size(), false);
        // 3. set influence of this joint to 1
        for(int& v_id : unhandledVertices)
        {
            skeletalMesh->m_VertexWeights[v_id][joint->GetID()] = 1.0;
            handled[v_id] = true;
        }

        // 4. handle all neighbors based on heat diff method
        for(int& v_id : unhandledVertices)
        {
            HandleVertex(skeletalMesh, joint, mesh->m_Vertices[v_id].get(), 1.0, min_d, 0.01f, d2s, handled);
        }
    }

    // make the sum of weights for every vertices to 1
    for(std::vector<float>& weights : skeletalMesh->m_VertexWeights)
    {
        std::vector<std::pair<int, float>> temp_weights;
        temp_weights.reserve(weights.size());
        for(int i = 0; i < weights.size(); ++i)
        {
            temp_weights.emplace_back(i, weights[i]);
            weights[i] = 0.f;
        }

        std::sort(temp_weights.begin(), temp_weights.end(),
                  [](const std::pair<int, float>& a, const std::pair<int, float>& b){
            return a.second > b.second;
        });
        float sum = 0.f;
        int max = (temp_weights.size() < 4 ? temp_weights.size() : 4);
        for(int i = 0; i < max; ++i)
        {
            sum += temp_weights[i].second;
        }
        for(int i = 0; i < max; ++i)
        {
            weights[temp_weights[i].first] = temp_weights[i].second / sum;
        }
    }
}

Vertex* SkeletalMesh::CloestVertex(Joint* joint,
                                    const std::vector<std::unique_ptr<Vertex>>& vertices,
                                    std::vector<float>& distance2s)
{
    const glm::vec3& j_pos = joint->getOverallTransformation().GetTranslation();
    float min_dis2 = std::numeric_limits<float>::max();
    Vertex* cloest_v = nullptr;
    for(const std::unique_ptr<Vertex>& v : vertices)
    {
        const glm::vec3& pos = v->GetPosition();
        float d2 = glm::length2(j_pos - pos);
        if(d2 < min_dis2)
        {
            min_dis2 = d2;
            cloest_v = v.get();
        }
        distance2s.emplace_back(d2);
    }

    return cloest_v;
}

void SkeletalMesh::HandleVertex(SkeletalMesh* skeletalMesh,
                                Joint* joint,
                                Vertex* v,
                                float weight,
                                const float& originDistance,
                                const float& threshold,
                                const std::vector<float>& d2s,
                                std::vector<bool>& handled)
{
    // assign weight for this vertex
    skeletalMesh->m_VertexWeights[v->GetIndex()][joint->GetID()] = weight;

    // iterate neighbors
    HalfEdge* e = v->GetHalfEdge();
    do
    {
        e = e->GetNext();

        Vertex* neighbor = e->GetVertex();
        if(!handled[neighbor->GetIndex()])
        {
            // compute falloff diffusion value
            float d = glm::sqrt(d2s[neighbor->GetIndex()]);
            float falloff_weight = weight - 0.1f * pow((d - originDistance)/ d, 3);

            // hanlde neighbor if falloff > given threshold
            if(falloff_weight > threshold)
            {
                handled[neighbor->GetIndex()] = true;
                HandleVertex(skeletalMesh, joint, neighbor, falloff_weight, originDistance, threshold, d2s, handled);
            }
        }
        e = e->GetSymmetric();
    }
    while(e != v->GetHalfEdge());
}

void SkeletalMesh::GetFirstKWeight(std::vector<float>& weights,
                                   const int& k,
                                   glm::vec4& weight,
                                   glm::ivec4& id)
{
    int count = 0;
    for(int i = 0; i < weights.size(); ++i)
    {
        if( weights[i] > 0.f)
        {
            weight[count] = weights[i];
            id[count++] = i;
            if(count == k)
            {
                return;
            }
        }
    }
}

void SkeletalMesh::HelpCreate(SkeletalMesh* skeletalMesh,
                              Joint* selectedJoint,
                              std::vector<glm::vec4>& pos,
                              std::vector<glm::vec4>& nor,
                              std::vector<glm::vec4>& col,
                              std::vector<glm::vec4>& weights,
                              std::vector<glm::ivec4>& ids,
                              std::vector<GLuint>& idx)
{
    Mesh* mesh = skeletalMesh->m_Mesh;
    for(std::unique_ptr<Face>& face : mesh->m_Faces)
    {
        glm::vec3 normal = face->GetNormal();
        HalfEdge* e = face->GetHalfEdge();
        GLuint start = pos.size();
        do
        {
            Vertex* v = e->GetVertex();
            // position, normal, color
            pos.emplace_back(glm::vec4(v->GetPosition(), 1.0));
            nor.emplace_back(glm::vec4(normal, 0.0));
            if(selectedJoint != nullptr)
            {
                const float& w = skeletalMesh->m_VertexWeights[v->GetIndex()][selectedJoint->GetID()];
                col.emplace_back(glm::vec4(w, 0.f, 1.0 - w, 1.f));
            }
            else
            {
                col.emplace_back(glm::vec4(0.66f, 0.66f, 0.66f, 1.f));
            }
            glm::vec4 weight(0);
            glm::ivec4 id(-1);
            GetFirstKWeight(skeletalMesh->m_VertexWeights[v->GetIndex()], 4, weight, id);
            weights.emplace_back(weight);
            ids.emplace_back(id);
            e = e->GetNext();
        }
        while(e != face->GetHalfEdge());
        for(GLuint i = start + 1; i < pos.size() - 1; ++i)
        {
            idx.emplace_back(start);
            idx.emplace_back(i);
            idx.emplace_back(i + 1);
        }
    }
}

SkeletalMesh::SkeletalMesh(OpenGLContext* context)
    :Drawable(context),
      m_Binded(false),
      m_Mesh(nullptr),
      m_Skeleton(nullptr)
{}

void SkeletalMesh::CreateWithSelectedJoint(Joint* joint)
{
    std::vector<glm::vec4> positions;
    std::vector<glm::vec4> normals;
    std::vector<glm::vec4> colors;
    std::vector<GLuint> indices;
    std::vector<glm::vec4> jointWeights;
    std::vector<glm::ivec4> jointIDs;

    HelpCreate(this, joint, positions, normals, colors, jointWeights, jointIDs, indices);

    count = indices.size();

    bindIdx();
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    bindPos();
    mp_context->glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec4), positions.data(), GL_STATIC_DRAW);

    bindNor();
    mp_context->glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec4), normals.data(), GL_STATIC_DRAW);

    bindCol();
    mp_context->glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec4), colors.data(), GL_STATIC_DRAW);

    bindID();
    mp_context->glBufferData(GL_ARRAY_BUFFER, jointIDs.size() * sizeof(glm::ivec4), jointIDs.data(), GL_STATIC_DRAW);

    bindWeight();
    mp_context->glBufferData(GL_ARRAY_BUFFER, jointWeights.size() * sizeof(glm::vec4), jointWeights.data(), GL_STATIC_DRAW);
}

void SkeletalMesh::create()
{
    CreateWithSelectedJoint(nullptr);
}

GLenum SkeletalMesh::drawMode()
{
    return GL_TRIANGLES;
}

void SkeletalMesh::Bind(Mesh* mesh, Skeleton* skeleton)
{
    if(mesh != nullptr && skeleton != nullptr)
    {
        m_Binded = true;
        m_Mesh = mesh;
        m_Skeleton = skeleton;
        InitJointWeights();
        skeleton->ComputeBind();
        SkeletalMesh::HeatDiffusionSkinning(this);
        create();
    }
}

void SkeletalMesh::UnBind()
{
    m_VertexWeights.clear();
    m_Binded = false;
    count = 0;
}

// drawable
void SkeletalMesh::Init()
{
    Drawable::Init();
    generateID();
    generateWeight();
}

void SkeletalMesh::generateID()
{
    IDBound = true;
    mp_context->glGenBuffers(1, &bufID);
}

void SkeletalMesh::generateWeight()
{
    WeightBound = true;
    mp_context->glGenBuffers(1, &bufWeight);
}

bool SkeletalMesh::bindID()
{
    if(IDBound)
    {
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufID);
    }
    return IDBound;
}

bool SkeletalMesh::bindWeight()
{
    if(WeightBound)
    {
        mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufWeight);
    }
    return WeightBound;
}

void SkeletalMesh::InitJointWeights()
{
    m_VertexWeights.clear();
    if(m_Binded)
    {
        for(unsigned int i = 0; i < m_Mesh->m_Vertices.size(); ++i)
        {
            std::vector<float> weights;
            weights.resize(m_Skeleton->GetJointsCount(), 0.0);
            m_VertexWeights.emplace_back(std::move(weights));
        }
    }
}

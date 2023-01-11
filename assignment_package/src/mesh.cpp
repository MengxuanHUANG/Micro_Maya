#include "mesh.h"
#include "halfedge.h"
#include "vertex.h"
#include "face.h"

#include <QFile>
#include <iostream>
#include <glm/gtc/random.hpp>
#include <map>
#include <math.h>

#ifndef MOD
    #define MOD(a, m) (((a) % (m) + (m)) % (m))
#endif

#ifndef VERTEX_INDEX
    #define VERTEX_INDEX 0
    #define UV_INDEX 1
    #define NORMAL_INDEX 2
#endif

namespace mymath
{
    template<typename T, typename I>
    T HashPair(const I& a, const I& b)
    {
        return (a << (sizeof(I) << 2)) + b;
    }

    glm::vec3 DeCasteljau(const glm::vec3& b0,
                          const glm::vec3& b1,
                          const glm::vec3& b2,
                          const glm::vec3& b3,
                          const float u)
    {
        glm::vec3 b01 = glm::mix(b0, b1, u);
        glm::vec3 b11 = glm::mix(b1, b2, u);
        glm::vec3 b21 = glm::mix(b2, b3, u);

        glm::vec3 b02 = glm::mix(b01, b11, u);
        glm::vec3 b12 = glm::mix(b11, b21, u);

        return glm::mix(b02, b12, u);
    }

}

void Mesh::HelpCreate(Mesh* mesh,
                      std::vector<glm::vec4>& pos,
                      std::vector<glm::vec4>& nor,
                      std::vector<glm::vec4>& col,
                      std::vector<GLuint>& idx)
{
    for(std::unique_ptr<Face>& face : mesh->m_Faces)
    {
        glm::vec3 normal = face->GetNormal();
        HalfEdge* e = face->GetHalfEdge();
        GLuint start = pos.size();
        do
        {
            pos.emplace_back(glm::vec4(e->GetVertex()->GetPosition(), 1.0));
            nor.emplace_back(glm::vec4(normal, 0.0));
            col.emplace_back(glm::vec4(face->GetColor(), 1.0));
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

Mesh::Mesh(OpenGLContext* context)
    :Drawable(context)
{}

void Mesh::LoadOBJ(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      std::cout << "Can't open \n" << path.toStdString() << std::endl;
    }
    else
    {
        QTextStream in(&file);
        std::vector<glm::vec2> vt;
        std::vector<glm::vec3> vn;
        std::unordered_map<size_t, HalfEdge*> halfEdges;
        QString text;
        while(in.readLineInto(&text))
        {
            if(text.startsWith('#'))
            {
                continue;
            }
            else if(text.startsWith("vt"))
            {
                QStringList list = text.split(' ');
                glm::vec2 tex_uv;
                for(int i = 0; i < 2; ++i)
                {
                    tex_uv[i] = list[i + 1].toFloat();
                }
                vt.emplace_back(tex_uv);
            }
            else if(text.startsWith("vn"))
            {
                QStringList list = text.split(' ');
                glm::vec3 nor;
                for(int i = 0; i < 3; ++i)
                {
                    nor[i] = list[i + 1].toFloat();
                }
                vn.emplace_back(nor);
            }
            else if(text.startsWith('v'))
            {
                QStringList list = text.split(' ');
                glm::vec3 pos;
                for(int i = 0; i < 3; ++i)
                {
                    pos[i] = list[i + 1].toFloat();
                }
                m_Vertices.emplace_back(std::make_unique<Vertex>(pos));
            }
            else if(text.startsWith('f'))
            {
                QStringList list = text.split(' ');
                glm::vec3 color = {glm::linearRand(0.f, 1.f),
                                   glm::linearRand(0.f, 1.f),
                                   glm::linearRand(0.f, 1.f)};
                std::unique_ptr<Face> face = std::make_unique<Face>(color);

                // construct halfEdges
                int edgeCount = 0;
                for(int i = 1; i < list.size(); ++i, ++edgeCount)
                {
                    QStringList pos_nor_tex = list[i].split('/');
                    std::unique_ptr<HalfEdge> e = std::make_unique<HalfEdge>();
                    e->SetFace(face.get());
                    e->SetVertex(m_Vertices[pos_nor_tex[VERTEX_INDEX].toInt() - 1].get());
                    e->SetUV(vt[pos_nor_tex[UV_INDEX].toInt() - 1]);
                    e->SetNormal(vn[pos_nor_tex[NORMAL_INDEX].toInt() - 1]);
                    m_HalfEdges.emplace_back(std::move(e));
                }
                // setup connection
                for(int i = 0; i < edgeCount; ++i)
                {
                    HalfEdge* e = m_HalfEdges[m_HalfEdges.size() - edgeCount + i].get(); // current edge
                    Vertex* preV = m_HalfEdges[m_HalfEdges.size() - edgeCount + MOD(i - 1, edgeCount)]->GetVertex(); // previous vertex
                    e->SetNext(m_HalfEdges[m_HalfEdges.size() - edgeCount + MOD(i + 1, edgeCount)].get()); // set next edge

                    size_t pre_id = preV->GetIndex();
                    size_t current_id = e->GetVertex()->GetIndex();
                    size_t key = mymath::HashPair<size_t, size_t>(glm::min(pre_id, current_id), glm::max(pre_id, current_id)); // hash number pair
                    auto it = halfEdges.find(key);
                    if(it != halfEdges.end())
                    {
                        e->SetSymmetric(it->second);
                        halfEdges.erase(it);
                    }
                    else
                    {
                        halfEdges.emplace(key, e);
                    }
                }
                m_Faces.emplace_back(std::move(face));
            }
        }
    }
}

void Mesh::ReleaseOBJ()
{
    m_Faces.clear();
    m_HalfEdges.clear();
    m_Vertices.clear();
}

void Mesh::create()
{
    std::vector<glm::vec4> positions;
    std::vector<glm::vec4> normals;
    std::vector<glm::vec4> colors;
    std::vector<GLuint> indices;

    HelpCreate(this, positions, normals, colors, indices);
    count = indices.size();

    bindIdx();
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    bindPos();
    mp_context->glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec4), positions.data(), GL_STATIC_DRAW);

    bindNor();
    mp_context->glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec4), normals.data(), GL_STATIC_DRAW);

    bindCol();
    mp_context->glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec4), colors.data(), GL_STATIC_DRAW);
}

GLenum Mesh::drawMode()
{
    return GL_TRIANGLES;
}

// mesh editing operations
Vertex* Mesh::DivideEdge(HalfEdge* e, const glm::vec3& pos)
{
    m_Vertices.emplace_back(std::make_unique<Vertex>(pos));
    Vertex* v = m_Vertices[m_Vertices.size() - 1].get();

    std::array<HalfEdge*, 2> edges {e, e->GetSymmetric()};
    for(int i = 0; i < 2; ++i)
    {
        std::unique_ptr<HalfEdge> newEdge = std::make_unique<HalfEdge>();
        newEdge->SetVertex(v);
        newEdge->SetFace(edges[i]->GetFace());
        newEdge->SetNext(edges[i]);
        newEdge->SetNormal(edges[i]->GetNormal());

        HalfEdge* current_e = edges[i];
        while(current_e->GetNext() != edges[i])
        {
            current_e = current_e->GetNext();
        }
        current_e->SetNext(newEdge.get());

        newEdge->SetSymmetric(edges[i ^ 1]);
        edges[i ^ 1]->SetSymmetric(newEdge.get());

        m_HalfEdges.emplace_back(std::move(newEdge));
    }

    return v;
}

Vertex* Mesh::DivideEdge(HalfEdge* e, const float& t)
{
    glm::vec3& p1 = e->GetSymmetric()->GetVertex()->GetPosition();
    glm::vec3& p2 = e->GetVertex()->GetPosition();

    return DivideEdge(e, p1 * t + p2 * (1.f - t));
}

void Mesh::TriangulateFace(Face* f)
{
    HalfEdge* e = f->GetHalfEdge();
    HalfEdge* current_e = e->GetNext();
    while(current_e->GetNext()->GetNext() != e)
    {
        std::unique_ptr<Face> newFace = std::make_unique<Face>();
        glm::vec3 color = {glm::linearRand(0.f, 1.f),
                           glm::linearRand(0.f, 1.f),
                           glm::linearRand(0.f, 1.f)};
        newFace->SetColor(color);
        current_e->SetFace(newFace.get());
        current_e->GetNext()->SetFace(newFace.get());

        std::unique_ptr<HalfEdge> newEdge = std::make_unique<HalfEdge>();
        std::unique_ptr<HalfEdge> symEdge = std::make_unique<HalfEdge>();

        newEdge->SetNormal(current_e->GetNormal());
        symEdge->SetNormal(e->GetNormal());

        //set Face
        newEdge->SetFace(newFace.get());
        symEdge->SetFace(f);
        //set Symmetric
        newEdge->SetSymmetric(symEdge.get());
        //set Vertex
        newEdge->SetVertex(e->GetVertex());
        symEdge->SetVertex(current_e->GetNext()->GetVertex());
        //set Next
        newEdge->SetNext(current_e);
        symEdge->SetNext(current_e->GetNext()->GetNext());
        current_e->GetNext()->SetNext(newEdge.get());
        e->SetNext(symEdge.get());

        current_e = symEdge.get();

        // add to vector
        m_Faces.emplace_back(std::move(newFace));
        m_HalfEdges.emplace_back(std::move(newEdge));
        m_HalfEdges.emplace_back(std::move(symEdge));
    }
}

void Mesh::SeperateFace(Face* f)
{
    HalfEdge* e = f->GetHalfEdge();
    Vertex* lastV = nullptr;
    HalfEdge* startE = nullptr;
    HalfEdge* lastE = nullptr;
    // seperate original face and create new faces
    do
    {
        //create new vertex for seperating face
        Vertex* oldVertex = e->GetVertex(); // original vertex
        std::unique_ptr<Vertex> newVertex = std::make_unique<Vertex>(oldVertex->GetPosition());
        e->SetVertex(newVertex.get()); // set new vertex to the half-edge instead of modifing the original vertex

        // create new face to connect the extruded face to original face
        std::unique_ptr<Face> newFace = std::make_unique<Face>();
        glm::vec3 color = {glm::linearRand(0.f, 1.f),
                           glm::linearRand(0.f, 1.f),
                           glm::linearRand(0.f, 1.f)};
        newFace->SetColor(color);

        // create 4 new halfedges to connect the extruded face to original face
        std::array<HalfEdge*, 4> newEdges;
        for(int i = 0; i < 4; ++i)
        {
            std::unique_ptr<HalfEdge> newEdge = std::make_unique<HalfEdge>();
            newEdge->SetFace(newFace.get());
            newEdges[i] = newEdge.get();
            m_HalfEdges.emplace_back(std::move(newEdge));
        }

        // set vertex for halfedges
        newEdges[0]->SetVertex(oldVertex);
        newEdges[1]->SetVertex(newVertex.get());
        newEdges[3]->SetVertex(oldVertex);
        // set next for halfedges
        newEdges[0]->SetNext(newEdges[1]);
        newEdges[1]->SetNext(newEdges[2]);
        newEdges[2]->SetNext(lastE); // lastE can be nullptr
        if(lastE != nullptr)
        {
            newEdges[2]->SetVertex(lastV);
            lastE->SetFace(newFace.get());
            lastE->SetNext(newEdges[0]);
        }
        else
        {
            startE = newEdges[0];
        }
        // set symmetric
        e->GetSymmetric()->SetSymmetric(newEdges[0]);
        e->SetSymmetric(newEdges[2]);
        newEdges[1]->SetSymmetric(newEdges[3]);

        lastV = newVertex.get();
        lastE = newEdges[3];
        m_Faces.emplace_back(std::move(newFace));
        m_Vertices.emplace_back(std::move(newVertex));
        e = e->GetNext();
    }
    while(e != f->GetHalfEdge());
    // connect last face
    lastE->SetFace(e->GetSymmetric()->GetFace());
    e->GetSymmetric()->SetNext(lastE);
    e->GetSymmetric()->SetVertex(lastV);
    lastE->SetNext(startE);
}

void Mesh::ExtrudeFace(Face* f, const float& d)
{
    SeperateFace(f);

    glm::vec3 normal = f->GetNormal();
    glm::vec3 extrusion = normal * d;
    HalfEdge* e = f->GetHalfEdge();
    // move face along its normal
    do
    {
        e->GetVertex()->SetPosition(e->GetVertex()->GetPosition() + extrusion);
        e = e->GetNext();
    }
    while(e != f->GetHalfEdge());
}

void Mesh::BevelFace(Face* f,
                     const unsigned int& segment,
                     const float& width,
                     const float& depth)
{
    std::vector<glm::vec3> originMoveVec;
    std::vector<glm::vec3> newMoveVec;

    // compute move direction of original vertices & new vertices
    HalfEdge* e = f->GetHalfEdge();
    do
    {
        glm::vec3 dir(0);
        HalfEdge* edge = e->GetNext()->GetSymmetric();
        unsigned int count = 0;
        do
        {
            edge = edge->GetNext();
            dir += edge->GetDirection();
            edge = edge->GetSymmetric();
            ++count;
        }
        while(edge->GetNext()->GetSymmetric() != e);
        glm::vec3 newMove = e->GetNext()->GetDirection() + e->GetSymmetric()->GetDirection();
        originMoveVec.emplace_back(dir * 0.5f);
        newMoveVec.emplace_back(newMove * 0.5f);

        e = e->GetNext();
    }
    while(e != f->GetHalfEdge());

    std::vector<Vertex*> originalVertices;
    for(unsigned int t = 0; t < segment; ++t)
    {
        float u = static_cast<float>(t) / static_cast<float>(segment);
        // record original vertices
        HalfEdge* e = f->GetHalfEdge();
        do
        {
            originalVertices.emplace_back(e->GetVertex());
            e = e->GetNext();
        }
        while(e != f->GetHalfEdge());

        //Seperate Face
        SeperateFace(f);

        // move original vertices
        for(unsigned int i = 0; i < originalVertices.size(); ++i)
        {
            // compute control points
            glm::vec3 b1 = originMoveVec[i] * width * pow(0.5f, depth);
            glm::vec3 b2 = newMoveVec[i] * width * pow(0.5f, depth);

            // interpolate between originMoveDir & newMoveDir
            glm::vec3& pos = originalVertices[i]->GetPosition();
            pos += mymath::DeCasteljau(originMoveVec[i] * width,
                                       b1,
                                       b2,
                                       newMoveVec[i] * width,
                                       u);
        }

        originalVertices.clear();
    }

    // shrink face
    e = f->GetHalfEdge();
    unsigned int i = 0;
    do
    {
        glm::vec3& pos = e->GetVertex()->GetPosition();
        pos = pos + width * newMoveVec[i++];
        e = e->GetNext();
    }
    while(e != f->GetHalfEdge());
}

void Mesh::InsertEdgeLoop(std::vector<HalfEdge*>& edgeLoop,
                    const float& t)
{
    std::vector<HalfEdge*> symmetrics;
    symmetrics.reserve(edgeLoop.size());
    // split edge
    for(HalfEdge* e : edgeLoop)
    {
        symmetrics.emplace_back(e->GetSymmetric());
        this->DivideEdge(e, t);
    }

    // connect created vertices
    for(int i = 0; i < edgeLoop.size(); ++i)
    {
        int preIndex = MOD(i - 1, static_cast<int>(edgeLoop.size()));
        HalfEdge* ei = edgeLoop[i];
        HalfEdge* ei_1 = edgeLoop[preIndex];

        // 1. create 2 half edges and 1 face
        std::unique_ptr<HalfEdge> newEdge_1 = std::make_unique<HalfEdge>();
        std::unique_ptr<HalfEdge> newEdge_2 = std::make_unique<HalfEdge>();
        std::unique_ptr<Face> newFace = std::make_unique<Face>();
        glm::vec3 color = {glm::linearRand(0.f, 1.f),
                           glm::linearRand(0.f, 1.f),
                           glm::linearRand(0.f, 1.f)};
        newFace->SetColor(color);

        //2. set up connection
        // set vertex
        newEdge_1->SetVertex(ei_1->GetSymmetric()->GetVertex());
        newEdge_2->SetVertex(ei->GetSymmetric()->GetVertex());
        // set symmetric
        newEdge_1->SetSymmetric(newEdge_2.get());

        // set next
        newEdge_1->SetNext(ei_1);
        newEdge_2->SetNext(symmetrics[i]);
        symmetrics[preIndex]->GetSymmetric()->SetNext(newEdge_2.get());
        ei->GetSymmetric()->SetNext(newEdge_1.get());

        // set face
        newEdge_2->SetFace(ei_1->GetFace());
        HalfEdge* e = newEdge_1.get();
        do
        {
            e->SetFace(newFace.get());
            e = e->GetNext();
        }
        while(e != newEdge_1.get());

        m_HalfEdges.emplace_back(std::move(newEdge_1));
        m_HalfEdges.emplace_back(std::move(newEdge_2));
        m_Faces.emplace_back(std::move(newFace));
    }
}

void Mesh::Subdivision(std::unordered_set<unsigned int>& sharpEdges,
                       std::unordered_set<unsigned int>& sharpVertices)
{
    std::vector<Vertex*> centroids;
    centroids.reserve(m_Faces.size());
    std::vector<bool> marked;

    //0. compute how many sharp edges bound to each vertex
    std::vector<unsigned int> sharpEdgeCount;
    std::unordered_map<unsigned int, glm::vec3> endpointSum; // end points sum for vertices

    size_t v_count = m_Vertices.size(); // max index of original vertices
    sharpEdgeCount.resize(v_count, 0);

    marked.resize(m_HalfEdges.size(), false);
    for(const unsigned int& id: sharpEdges)
    {
        HalfEdge* e = m_HalfEdges[id].get();
        if(!marked[e->GetIndex()])
        {
            marked[e->GetIndex()] = true;
            marked[e->GetSymmetric()->GetIndex()] = true;

            unsigned int index = e->GetVertex()->GetIndex();
            unsigned int symIndex = e->GetSymmetric()->GetVertex()->GetIndex();

            ++sharpEdgeCount[index];
            auto it = endpointSum.find(index);
            if(it == endpointSum.end())
            {
                endpointSum.emplace(index,
                                    0.75f * e->GetVertex()->GetPosition() + 0.125f * e->GetSymmetric()->GetVertex()->GetPosition());
            }
            else
            {
                it->second += 0.125f * e->GetSymmetric()->GetVertex()->GetPosition();
            }

            ++sharpEdgeCount[symIndex];

            it = endpointSum.find(symIndex);
            if(it == endpointSum.end())
            {
                endpointSum.emplace(symIndex,
                                    0.75f * e->GetSymmetric()->GetVertex()->GetPosition() + 0.125f * e->GetVertex()->GetPosition());
            }
            else
            {
                it->second += 0.125f * e->GetVertex()->GetPosition();
            }
        }
    }

    // 1. calculate centroid for faces
    for(std::unique_ptr<Face>& f : m_Faces)
    {
        glm::vec3 pos{0};
        HalfEdge* e = f->GetHalfEdge();
        float count = 0.f;
        do
        {
            count += 1.f;
            pos += e->GetVertex()->GetPosition();
            e = e->GetNext();
        }
        while(e != f->GetHalfEdge());
        std::unique_ptr<Vertex> newVertex = std::make_unique<Vertex>(pos / count);
        centroids.emplace_back(newVertex.get());
        m_Vertices.emplace_back(std::move(newVertex));
    }

    marked.clear();
    marked.resize(m_HalfEdges.size(), false);

    // 2. compute smoothed midpoint of each edge
    const int orginalEdgeCount = m_HalfEdges.size();
    for(int i = 0; i < orginalEdgeCount; ++i)
    {
        HalfEdge* e = m_HalfEdges[i].get();
        if(!marked[e->GetIndex()])
        {
            marked[e->GetIndex()] = true;
            marked[e->GetSymmetric()->GetIndex()] = true;
            if(sharpEdges.find(e->GetIndex()) == sharpEdges.end())
            {
                glm::vec3 pos = (e->GetVertex()->GetPosition() +
                                 e->GetSymmetric()->GetVertex()->GetPosition() +
                                 centroids[e->GetFace()->GetIndex()]->GetPosition() +
                                 centroids[e->GetSymmetric()->GetFace()->GetIndex()]->GetPosition()) / 4.0f;
                this->DivideEdge(e, pos);
            }
            else
            {
                glm::vec3 pos = (e->GetVertex()->GetPosition() +
                                 e->GetSymmetric()->GetVertex()->GetPosition()) / 2.0f;
                this->DivideEdge(e, pos);
            }
        }
    }

    // 3. smooth the original vertices
    for(size_t i = 0; i < v_count; ++i)
    {
        if(sharpEdgeCount[i] > 2 ||
           sharpVertices.find(m_Vertices[i]->GetIndex()) != sharpVertices.end()) // sharp vertex
        {
            continue;
        }
        else if(sharpEdgeCount[i] == 2)
        {
            m_Vertices[i]->SetPosition(endpointSum.find(m_Vertices[i]->GetIndex())->second);
        }
        else
        {
            glm::vec3& pos = m_Vertices[i]->GetPosition();

            int midPoint_count = 0;
            glm::vec3 midPoints_sum(0);
            glm::vec3 centroids_sum(0);

            HalfEdge* e = m_Vertices[i]->GetHalfEdge();
            do
            {
                e = e->GetNext();
                midPoints_sum += e->GetVertex()->GetPosition();
                centroids_sum += centroids[e->GetFace()->GetIndex()]->GetPosition();
                e = e->GetSymmetric();

                ++midPoint_count;
            }
            while(e != m_Vertices[i]->GetHalfEdge());

            float n = static_cast<float>(midPoint_count);
            pos = (n - 2.f) * pos / n +
                    midPoints_sum / (n * n) +
                    centroids_sum / (n * n);
        }
    }

    // 4. split faces
    const int orginalFaceCount = m_Faces.size();
    for(int i = 0; i < orginalFaceCount; ++i)
    {
        Vertex* centroid = centroids[m_Faces[i]->GetIndex()];

        HalfEdge* firstEdge = m_Faces[i]->GetHalfEdge();
        HalfEdge* e = firstEdge;
        HalfEdge* lastSecondEdge = nullptr;
        Face* lastFace = m_Faces[i].get();
        do
        {
            HalfEdge* next = e->GetNext()->GetNext();

            // create first
            std::unique_ptr<HalfEdge> newEdge = std::make_unique<HalfEdge>();
            // create new face
            std::unique_ptr<Face> newFace = std::make_unique<Face>();
            glm::vec3 color = {glm::linearRand(0.f, 1.f),
                               glm::linearRand(0.f, 1.f),
                               glm::linearRand(0.f, 1.f)};
            newFace->SetColor(color);

            // create second edge & setup connection
            std::unique_ptr<HalfEdge> symEdge = std::make_unique<HalfEdge>();
            symEdge->SetSymmetric(newEdge.get());
            // set normal
            newEdge->SetNormal(e->GetNormal());
            symEdge->SetNormal(e->GetNext()->GetNormal());
            // set vertex
            newEdge->SetVertex(centroid);
            symEdge->SetVertex(e->GetVertex());
            // set face
            newEdge->SetFace(lastFace);
            symEdge->SetFace(newFace.get());
            e->GetNext()->SetFace(newFace.get());
            e->GetNext()->GetNext()->SetFace(newFace.get());
            // set next
            newEdge->SetNext(lastSecondEdge);
            symEdge->SetNext(e->GetNext());
            e->SetNext(newEdge.get()); // !!!IMPORTANT: must be set at last

            lastFace = newFace.get();
            lastSecondEdge = symEdge.get();
            m_Faces.emplace_back(std::move(newFace));
            m_HalfEdges.emplace_back(std::move(newEdge));
            m_HalfEdges.emplace_back(std::move(symEdge));
            e = next;
        }
        while(e->GetNext()->GetNext() != firstEdge);

        // set up last face
        // create first&second edge
        std::unique_ptr<HalfEdge> newEdge = std::make_unique<HalfEdge>();
        std::unique_ptr<HalfEdge> symEdge = std::make_unique<HalfEdge>();
        symEdge->SetSymmetric(newEdge.get());
        // set normal
        newEdge->SetNormal(e->GetNormal());
        symEdge->SetNormal(e->GetNext()->GetNormal());
        // set vertex
        newEdge->SetVertex(centroid);
        symEdge->SetVertex(e->GetVertex());
        // set face
        newEdge->SetFace(lastFace);
        symEdge->SetFace(m_Faces[i].get());
        // set next
        newEdge->SetNext(lastSecondEdge);
        symEdge->SetNext(e->GetNext());
        firstEdge->GetNext()->SetNext(symEdge.get());
        e->SetNext(newEdge.get()); // !!!IMPORTANT: must be set at last

        m_HalfEdges.emplace_back(std::move(newEdge));
        m_HalfEdges.emplace_back(std::move(symEdge));
    }

    sharpVertices.clear();
    sharpEdges.clear();
}

#include "halfedgedisplay.h"
#include "halfedge.h"

HalfEdgeDisplay::HalfEdgeDisplay(OpenGLContext* context)
    :Drawable(context),
     m_PreviousSelectedHalfEdge(nullptr),
     m_CurrentSelectedHalfEdge(nullptr)
{}

void HalfEdgeDisplay::SelectHalfEdge(HalfEdge* e)
{
    m_SelectedHalfEdges.emplace(e);
    m_PreviousSelectedHalfEdge = m_CurrentSelectedHalfEdge;
    m_CurrentSelectedHalfEdge = e;
}

void HalfEdgeDisplay::ClearSelection()
{
    m_SelectedHalfEdges.clear();
    m_CurrentSelectedHalfEdge = nullptr;
    m_PreviousSelectedHalfEdge = nullptr;
    count = 0;
}

void HalfEdgeDisplay::create()
{
    if(!m_SelectedHalfEdges.empty())
    {
        std::vector<glm::vec4> positions;
        std::vector<glm::vec4> colors;
        std::vector<GLuint> indices;
        GLuint i = 0;
        for(HalfEdge* e : m_SelectedHalfEdges)
        {
            indices.emplace_back(2 * i);
            indices.emplace_back(2 * i + 1);
            positions.emplace_back(glm::vec4(e->GetVertex()->GetPosition(), 1.f));
            positions.emplace_back(glm::vec4(e->GetSymmetric()->GetVertex()->GetPosition(), 1.f));
            colors.emplace_back(glm::vec4(1, 0, 0, 1));
            colors.emplace_back(glm::vec4(1, 1, 0, 1));
            ++i;
        }

        generateIdx();
        bindIdx();
        mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        generatePos();
        bindPos();
        mp_context->glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec4), positions.data(), GL_STATIC_DRAW);

        generateCol();
        bindCol();
        mp_context->glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec4), colors.data(), GL_STATIC_DRAW);

        count = indices.size();
    }
    else
    {
        count = 0;
    }
}

GLenum HalfEdgeDisplay::drawMode()
{
    return GL_LINES;
}

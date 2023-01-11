#include "vertexdisplay.h"
#include "vertex.h"

VertexDisplay::VertexDisplay(OpenGLContext* context)
    :Drawable(context), m_CurrentSelectedVertex(nullptr)
{}

void VertexDisplay::SelectVertex(Vertex* v)
{
    m_SelectedVertices.emplace(v);
    m_CurrentSelectedVertex = v;
}

void VertexDisplay::ClearSelection()
{
    m_SelectedVertices.clear();
    m_CurrentSelectedVertex = nullptr;
    count = 0;
}

void VertexDisplay::create()
{
    if(!m_SelectedVertices.empty())
    {
        std::vector<glm::vec4> positions;
        std::vector<glm::vec4> colors;
        std::vector<GLuint> indices;
        GLuint i = 0;
        for(Vertex* v : m_SelectedVertices)
        {
            indices.emplace_back(i);
            positions.emplace_back(glm::vec4(v->GetPosition(), 1.f));
            colors.emplace_back(glm::vec4(1));
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

GLenum VertexDisplay::drawMode()
{
    return GL_POINTS;
}

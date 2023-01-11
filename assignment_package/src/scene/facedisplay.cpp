#include "facedisplay.h"

#include "face.h"
#include "halfedge.h"
#include <iostream>

FaceDisplay::FaceDisplay(OpenGLContext* context)
    :Drawable(context), m_CurrentSelectedFace(nullptr)
{}

void FaceDisplay::SelectFace(Face* f)
{
    m_SelectedFaces.emplace(f);
    m_CurrentSelectedFace = f;
    this->create();
}

void FaceDisplay::ClearSelection()
{
    m_SelectedFaces.clear();
    m_CurrentSelectedFace = nullptr;
    count = 0;
}

void FaceDisplay::create()
{
    if(!m_SelectedFaces.empty())
    {
        std::vector<glm::vec4> positions;
        std::vector<glm::vec4> colors;
        std::vector<GLuint> indices;

        for(Face* f : m_SelectedFaces)
        {
            HalfEdge* current_e = f->GetHalfEdge();
            glm::vec4 color = glm::vec4(1.0) - glm::vec4(f->GetColor(), 0);
            GLuint count = positions.size();
            do
            {
                positions.emplace_back(glm::vec4(current_e->GetVertex()->GetPosition(), 1.0));
                colors.emplace_back(color);
                current_e = current_e->GetNext();
            }
            while(current_e != f->GetHalfEdge());

            for(GLuint i = count; i < positions.size() - 1; ++i)
            {
                indices.emplace_back(i);
                indices.emplace_back(i + 1);
            }
            indices.emplace_back(positions.size() - 1);
            indices.emplace_back(count);
        }

        count = indices.size();

        bindIdx();
        mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

        bindPos();
        mp_context->glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec4), positions.data(), GL_STATIC_DRAW);

        bindCol();
        mp_context->glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec4), colors.data(), GL_STATIC_DRAW);
    }
    else
    {
        count = 0;
    }
}

GLenum FaceDisplay::drawMode()
{
    return GL_LINES;
}

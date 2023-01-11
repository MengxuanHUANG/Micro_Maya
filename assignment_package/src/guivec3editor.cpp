#include "guivec3editor.h"

GUIVec3Editor::GUIVec3Editor(QDoubleSpinBox* xSpin, QDoubleSpinBox* ySpin, QDoubleSpinBox* zSpin)
    :m_XSpin(xSpin), m_YSpin(ySpin), m_ZSpin(zSpin)
{}

GUIVec3Editor::GUIVec3Editor()
    :GUIVec3Editor(nullptr, nullptr, nullptr)
{}

void GUIVec3Editor::slot_XValueChanged(double x)
{
    glm::vec3 value{x, m_YSpin->value(), m_ZSpin->value()};
    sig_Vec3Changed(value);
}

void GUIVec3Editor::slot_YValueChanged(double y)
{
    glm::vec3 value{m_XSpin->value(), y, m_ZSpin->value()};
    sig_Vec3Changed(value);
}

void GUIVec3Editor::slot_ZValueChanged(double z)
{
    glm::vec3 value{m_XSpin->value(), m_YSpin->value(), z};
    sig_Vec3Changed(value);
}

void GUIVec3Editor::slot_SetEnabled(bool enabled)
{
    SetEnabled(enabled);
}

void GUIVec3Editor::slot_SetValue(const glm::vec3& value, bool blockSignal)
{
    SetValue(value, blockSignal);
}

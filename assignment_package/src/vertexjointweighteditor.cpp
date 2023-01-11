#include "vertexjointweighteditor.h"

VertexJointWeightEditor::VertexJointWeightEditor()
{}

void VertexJointWeightEditor::Init(const std::array<QComboBox*, 4>& jointBoxes,
                                   const std::array<QDoubleSpinBox*, 4>& weightSpins)
{
    for(int i = 0; i < 4; ++i)
    {
        m_JointWeightEditors[i].Init(i, jointBoxes[i], weightSpins[i]);
        connect(&(m_JointWeightEditors[i]),
                SIGNAL(sig_JointChanged(int, const QString&)),
                this,
                SLOT(slot_JointChanged(int, const QString&)));

        connect(&(m_JointWeightEditors[i]),
                SIGNAL(sig_WeightChanged(int, double)),
                this,
                SLOT(slot_WeightChanged(int, double)));
    }
}

void VertexJointWeightEditor::SetValue(const std::array<QString, 4>& joints,
                                       const std::array<double, 4>& weights,
                                       int jointCount,
                                       bool blockSignal)
{
    m_JointCount = jointCount;
    blockSignals(blockSignal);
    for(int i = 0; i < 4; ++i)
    {
        m_CurrentJoints[i] = joints[i];
    }
    UpdateAviableList();

    for(int i = 0; i < 4; ++i)
    {
        m_JointWeightEditors[i].SetValue(joints[i], weights[i], blockSignal);
    }

    blockSignals(false);
}

void VertexJointWeightEditor::SetJointNames(const QStringList& list)
{
    m_JointNames = list;
}

void VertexJointWeightEditor::UpdateAviableList()
{
    for(int i = 0; i < 4; ++i)
    {
        QStringList list = m_JointNames;
        list.append(m_CurrentJoints[i]);
        m_JointWeightEditors[i].SetJointBox(list);
    }
}

void VertexJointWeightEditor::ReplaceString(const QString& text, const QString& newText)
{
    for(QString& jointName : m_JointNames)
    {
        if(jointName == text)
        {
            jointName = newText;
            return;
        }
    }
}

void VertexJointWeightEditor::slot_JointChanged(int index, const QString& currentJoint)
{
    sig_JointChanged(m_CurrentJoints[index], currentJoint);
    if(currentJoint != "None")
    {
        ReplaceString(currentJoint, m_CurrentJoints[index]);
    }
    else
    {
        m_JointNames.append(m_CurrentJoints[index]);
    }
    m_CurrentJoints[index] = currentJoint;
    UpdateAviableList();
}

void VertexJointWeightEditor::slot_WeightChanged(int index, double weight)
{
    sig_WeightChanged(m_CurrentJoints[index], weight);
}

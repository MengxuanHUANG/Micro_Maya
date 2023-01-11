#include "jointweighteditor.h"

JointWeightEditor::JointWeightEditor(QComboBox* jointBox, QDoubleSpinBox* weightSpin)
    :m_JointComboBox(jointBox), m_WeightSpinBox(weightSpin)
{}

JointWeightEditor::JointWeightEditor()
    :JointWeightEditor(nullptr, nullptr)
{}

void JointWeightEditor::Init(int index, QComboBox* jointBox, QDoubleSpinBox* weightSpin)
{
    m_Index = index;
    m_JointComboBox = jointBox;
    m_WeightSpinBox = weightSpin;

    connect(m_JointComboBox,
            SIGNAL(currentTextChanged(const QString&)),
            this,
            SLOT(slot_JointChanged(const QString&)));

    connect(m_WeightSpinBox,
            SIGNAL(valueChanged(double)),
            this,
            SLOT(slot_WeightChanged(double)));
}

void JointWeightEditor::SetValue(const QString& jointName, double weight, bool blockSignal)
{
    blockSignals(blockSignal);
    m_JointComboBox->setCurrentText(jointName);
    m_WeightSpinBox->setValue(weight);
    blockSignals(false);
}

void JointWeightEditor::slot_JointChanged(const QString& jointName)
{
    sig_JointChanged(m_Index, jointName);
    m_WeightSpinBox->setEnabled(jointName != "None");
}

void JointWeightEditor::slot_WeightChanged(double weight)
{
    sig_WeightChanged(m_Index, weight);
}

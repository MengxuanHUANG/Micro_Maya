#ifndef VERTEXJOINTWEIGHTEDITOR_H
#define VERTEXJOINTWEIGHTEDITOR_H

#include <array>
#include "jointweighteditor.h"

class VertexJointWeightEditor : public QObject
{
    Q_OBJECT
public:
    VertexJointWeightEditor();

    void Init(const std::array<QComboBox*, 4>&, const std::array<QDoubleSpinBox*, 4>&);
    void SetValue(const std::array<QString, 4>&, const std::array<double, 4>&, int, bool);
    void SetJointNames(const QStringList& list);
    inline void BlockSignal(bool block) { blockSignals(block); }
    inline void SetDisabled(bool disabled)
    {
        for(JointWeightEditor& editor : m_JointWeightEditors)
        {
            editor.SetDisabled(disabled);
        }
    }
    inline void ClearValue()
    {
        for(JointWeightEditor& editor : m_JointWeightEditors)
        {
            editor.ClearValue();
        }
    }
signals:
    void sig_JointChanged(const QString& preJoint, const QString& currentJoint);
    void sig_WeightChanged(const QString& currentJoint, double weight);

protected:
    void UpdateAviableList();
    void ReplaceString(const QString&, const QString&);

private slots:
    void slot_JointChanged(int, const QString&);
    void slot_WeightChanged(int, double);

public:
    int m_JointCount;
    QStringList m_JointNames;
    std::array<QString, 4> m_CurrentJoints;
    std::array<JointWeightEditor, 4> m_JointWeightEditors;
};

#endif // VERTEXJOINTWEIGHTEDITOR_H

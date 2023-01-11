#ifndef JOINTWEIGHTEDITOR_H
#define JOINTWEIGHTEDITOR_H

#include <QComboBox>
#include <QDoubleSpinBox>

class JointWeightEditor : public QObject
{
    Q_OBJECT
public:
    JointWeightEditor(QComboBox*, QDoubleSpinBox*);
    JointWeightEditor();

    void Init(int, QComboBox*, QDoubleSpinBox*);

    inline void SetDisabled(bool disabled)
    {
        m_JointComboBox->setDisabled(disabled);
        m_WeightSpinBox->setDisabled(disabled);
    }

    inline void SetJointBox(QStringList& jointNames)
    {
        m_JointComboBox->blockSignals(true);

        QString text = m_JointComboBox->currentText();
        m_JointComboBox->clear();
        m_JointComboBox->addItem("None");
        m_JointComboBox->addItems(jointNames);
        m_JointComboBox->setCurrentText(text);

        m_JointComboBox->blockSignals(false);
    }
    inline void SetWeightLimits(double min, double max)
    {
        m_WeightSpinBox->setMinimum(min);
        m_WeightSpinBox->setMaximum(max);
    }
    inline void ClearValue()
    {
        m_JointComboBox->blockSignals(true);
        m_WeightSpinBox->blockSignals(true);
        // clear list
        m_JointComboBox->clear();
        m_JointComboBox->addItem("None");
        // set to default selection
        m_JointComboBox->setCurrentText("None");
        m_WeightSpinBox->setValue(0);

        m_WeightSpinBox->blockSignals(false);
        m_JointComboBox->blockSignals(false);
    }
    void SetValue(const QString&, double, bool blockSignal = false);

signals:
    void sig_JointChanged(int, const QString&);
    void sig_WeightChanged(int, double);

private slots:
    void slot_JointChanged(const QString&);
    void slot_WeightChanged(double);

protected:
    int m_Index;
    QComboBox* m_JointComboBox;
    QDoubleSpinBox* m_WeightSpinBox;
};

#endif // JOINTWEIGHTEDITOR_H

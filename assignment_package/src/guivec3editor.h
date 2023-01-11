#ifndef GUIVEC3EDITOR_H
#define GUIVEC3EDITOR_H

#include <QDoubleSpinBox>
#include <array>
#include <glm/glm.hpp>

class GUIVec3Editor : public QObject
{
    Q_OBJECT
public:
    GUIVec3Editor(QDoubleSpinBox* xSpin, QDoubleSpinBox* ySpin, QDoubleSpinBox* zSpin);
    GUIVec3Editor();

    inline void Init(QDoubleSpinBox* xSpin, QDoubleSpinBox* ySpin, QDoubleSpinBox* zSpin)
    {
        m_XSpin = xSpin;
        m_YSpin = ySpin;
        m_ZSpin = zSpin;

        connect(m_XSpin,
                SIGNAL(valueChanged(double)),
                this,
                SLOT(slot_XValueChanged(double)));

        connect(m_YSpin,
                SIGNAL(valueChanged(double)),
                this,
                SLOT(slot_YValueChanged(double)));

        connect(m_ZSpin,
                SIGNAL(valueChanged(double)),
                this,
                SLOT(slot_ZValueChanged(double)));
    }

    inline void BlockSignals(bool block)
    {
        m_XSpin->blockSignals(block);
        m_YSpin->blockSignals(block);
        m_ZSpin->blockSignals(block);
    }

    inline void SetValue(const glm::vec3& value, bool blockSignal = false)
    {
        BlockSignals(blockSignal);
        m_XSpin->setValue(value.x);
        m_YSpin->setValue(value.y);
        m_ZSpin->setValue(value.z);
        BlockSignals(false);
    }

    inline void SetEnabled(bool enabled)
    {
        m_XSpin->setEnabled(enabled);
        m_YSpin->setEnabled(enabled);
        m_ZSpin->setEnabled(enabled);
    }
signals:
    void sig_Vec3Changed(const glm::vec3&);

private slots:
    // receive signal from GUI
    void slot_XValueChanged(double);
    void slot_YValueChanged(double);
    void slot_ZValueChanged(double);

    void slot_SetEnabled(bool);
    void slot_SetValue(const glm::vec3& value, bool blockSignal = false);

protected:
    QDoubleSpinBox* m_XSpin;
    QDoubleSpinBox* m_YSpin;
    QDoubleSpinBox* m_ZSpin;
};

#endif // GUIVEC3EDITOR_H

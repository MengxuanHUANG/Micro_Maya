#include "menuitem.h"

MenuItem::MenuItem(const unsigned int& index)
    :m_Index(index)
{
    //QListWidgetItem::setText for QListWidgetItem
    QListWidgetItem::setText(QString::number(m_Index));
}

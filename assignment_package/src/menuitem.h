#ifndef MENUITEM_H
#define MENUITEM_H

#include <QListWidget>

class MenuItem : public QListWidgetItem
{
public:
    MenuItem(const unsigned int& index);

public:
    inline const int& GetIndex() const { return m_Index; }
    inline void SetIndex(const int& index) { m_Index = index; }

protected:
    // For debug only
    int m_Index;
};

#endif // MENUITEM_H

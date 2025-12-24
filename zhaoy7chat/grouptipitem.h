#ifndef GROUPTIPITEM_H
#define GROUPTIPITEM_H

#include <QWidget>
#include "listitembase.h"
namespace Ui {
class GroupTipItem;
}

class GroupTipItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit GroupTipItem(QWidget *parent = nullptr);
    ~GroupTipItem();
    QSize sizeHint() const override;
    void SetGroupTip(QString str); //新的朋友 联系人 这种字符串
private:
    Ui::GroupTipItem *ui;
    QString _tip;
};

#endif // GROUPTIPITEM_H

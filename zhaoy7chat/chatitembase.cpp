#include "chatitembase.h"
#include <QGridLayout>

ChatItemBase::ChatItemBase(ChatRole role, QWidget *parent):
    QWidget(parent), m_role(role)
{
    //name
    m_pNameLabel = new QLabel();
    m_pNameLabel->setObjectName("chat_user_name");
    QFont font("Microsoft YaHei");
    font.setPointSize(9);
    m_pNameLabel->setFont(font);
    m_pNameLabel->setFixedHeight(20);

    //icon
    m_pIconLabel    = new QLabel();
    m_pIconLabel->setScaledContents(true);
    m_pIconLabel->setFixedSize(42, 42);

    //bubble
    m_pBubble = new QWidget();

    //网格布局3*3
    QGridLayout* pGLayout = new QGridLayout();
    pGLayout->setVerticalSpacing(3);
    pGLayout->setHorizontalSpacing(3);
    pGLayout->setContentsMargins(3,3,3,3);

    //弹簧
    QSpacerItem* pSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    //判断角色
    if(m_role == ChatRole::Self){
        m_pNameLabel->setContentsMargins(0,0,8,0);
        m_pNameLabel->setAlignment(Qt::AlignRight);
        pGLayout->addWidget(m_pNameLabel,0,1,1,1);//row,col,rowSpan,colSpan  占一行一列
        pGLayout->addWidget(m_pIconLabel,0,2,2,1,Qt::AlignTop);//占两行一列
        pGLayout->addItem(pSpacer,1,0,1,1);
        pGLayout->addWidget(m_pBubble,1,1,1,1);
        pGLayout->setColumnStretch(0,2);//拉伸比例
        pGLayout->setColumnStretch(1,3);
    }else{
        m_pNameLabel->setContentsMargins(8,0,0,0);
        m_pNameLabel->setAlignment(Qt::AlignLeft);
        pGLayout->addWidget(m_pIconLabel,0,0,2,1,Qt::AlignTop);//占两行一列
        pGLayout->addWidget(m_pNameLabel,0,1,1,1);//row,col,rowSpan,colSpan  占一行一列
        pGLayout->addWidget(m_pBubble,1,1,1,1);
        pGLayout->addItem(pSpacer,2,2,1,1);
        pGLayout->setColumnStretch(1,3);//拉伸比例
        pGLayout->setColumnStretch(2,2);
    }
    this->setLayout(pGLayout);
}

void ChatItemBase::setUserName(const QString &name)
{
    m_pNameLabel->setText(name);
}

void ChatItemBase::setUserIcon(const QPixmap &icon)
{
    m_pIconLabel->setPixmap(icon);
}

void ChatItemBase::setWidget(QWidget *w)
{
    QGridLayout* pGlayout = (qobject_cast<QGridLayout*>)(this->layout());
    pGlayout->replaceWidget(m_pBubble, w);
    delete m_pBubble;//删除原先bubble 防止内存泄露
    m_pBubble = w;
}

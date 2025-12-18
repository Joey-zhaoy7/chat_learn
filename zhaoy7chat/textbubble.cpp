#include "textbubble.h"
#include <QTextDocument>
#include <QTextBlock>
#include <QFontMetricsF>
#include <QTimer>

TextBubble::TextBubble(ChatRole role, const QString &text, QWidget *parent)
    :BubbleFrame(role, parent)
{
    m_pTextEdit = new QTextEdit();
    m_pTextEdit->setReadOnly(true);
    m_pTextEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pTextEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pTextEdit->installEventFilter(this);
    // m_pTextEdit->setWordWrapMode(QTextOption::NoWrap);
    QFont font("Microsoft YaHei");
    font.setPointSize(12);
    m_pTextEdit->setFont(font);
    setPlainText(text);
    setWidget(m_pTextEdit);
    initStyleSheet();
    //延迟设置文本，确保parent布局完成 ？
    // QTimer::singleShot(0,this,[this](){
    //     if(this->parentWidget()){
    //         int maxWidth = this->parentWidget()->width() * 0.6 ;
    //         setPlainText(m_text,maxWidth);
    //     }else{
    //         setPlainText(m_text,300);
    //     }
    // });
}

bool TextBubble::eventFilter(QObject *o, QEvent *e)
{
    if(m_pTextEdit == o && e->type() == QEvent::Paint)
    {
        adjustTextHeight(); //PaintEvent中设置

        // int maxWidth = this->parentWidget()->width() * 0.6;
        // setPlainText(m_text,maxWidth);
    }
    return BubbleFrame::eventFilter(o, e);
}

void TextBubble::adjustTextHeight()
{
    qreal doc_margin = m_pTextEdit->document()->documentMargin();    //字体到边框的距离默认为4
    QTextDocument *doc = m_pTextEdit->document();
    qreal text_height = 0;
    //把每一段的高度相加=文本高
    for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next())
    {
        QTextLayout *pLayout = it.layout();
        QRectF text_rect = pLayout->boundingRect();                             //这段的rect
        text_height += text_rect.height();
    }
    int vMargin = this->layout()->contentsMargins().top();
    //设置这个气泡需要的高度 文本高+文本边距+TextEdit边框到气泡边框的距离
    setFixedHeight(text_height + doc_margin *2 + vMargin*2 );
}

void TextBubble::setPlainText(const QString &text)
{
    // QString wrappedText  = wrapTextTofitWidth(text, maxWidth);
    m_pTextEdit->setPlainText(text);
    //m_pTextEdit->setHtml(text);
    //找到段落中最大宽度
    qreal doc_margin = m_pTextEdit->document()->documentMargin();
    int margin_left = this->layout()->contentsMargins().left();
    int margin_right = this->layout()->contentsMargins().right();
    QFontMetricsF fm(m_pTextEdit->font());
    QTextDocument *doc = m_pTextEdit->document();
    int max_width = 0;
    //遍历每一段找到 最宽的那一段
    for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next())    //字体总长
    {
        int txtW = int(fm.horizontalAdvance(it.text()));
        max_width = max_width < txtW ? txtW : max_width;                 //找到最长的那段
    }
    //设置这个气泡的最大宽度 只需要设置一次
    setMaximumWidth(max_width + doc_margin * 2 + (margin_left + margin_right) + 1);        //设置最大宽度
    // setFixedWidth(maxWidth+doc_margin * 2 + (margin_left + margin_right));
}

// void TextBubble::setPlainText(const QString &text,int maxWidth)
// {
//     QString wrappedText  = wrapTextTofitWidth(text, maxWidth);
//     m_pTextEdit->setPlainText(wrappedText);
//     //m_pTextEdit->setHtml(text);
//     //找到段落中最大宽度
//     qreal doc_margin = m_pTextEdit->document()->documentMargin();
//     int margin_left = this->layout()->contentsMargins().left();
//     int margin_right = this->layout()->contentsMargins().right();
//     QFontMetricsF fm(m_pTextEdit->font());
//     QTextDocument *doc = m_pTextEdit->document();
//     int max_width = 0;
//     //遍历每一段找到 最宽的那一段
//     for (QTextBlock it = doc->begin(); it != doc->end(); it = it.next())    //字体总长
//     {
//         int txtW = int(fm.horizontalAdvance(it.text()));
//         max_width = max_width < txtW ? txtW : max_width;                 //找到最长的那段
//     }
//     //设置这个气泡的最大宽度 只需要设置一次
//     // setMaximumWidth(max_width + doc_margin * 2 + (margin_left + margin_right));        //设置最大宽度
//     setFixedWidth(maxWidth+doc_margin * 2 + (margin_left + margin_right));
// }


void TextBubble::initStyleSheet()
{
    m_pTextEdit->setStyleSheet("QTextEdit{background:transparent;border:none}");
}

QString TextBubble::wrapTextTofitWidth(const QString &text, int maxWidth)
{
    QFontMetricsF metrics(m_pTextEdit->font());
    QString result;
    QStringList lines = text.split('\n');
    for(const QString& line : lines){
        QString currentLine;
        for(int i{}; i<line.length(); i++){
            currentLine.append(line[i]);
            if(metrics.horizontalAdvance(currentLine) > maxWidth){
                currentLine.chop(1);
                result.append(currentLine + '\n');
                currentLine = line[i];
            }
        }
        result.append(currentLine + '\n');
    }
    return result.trimmed();
}

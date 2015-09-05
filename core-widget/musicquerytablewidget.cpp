#include "musicquerytablewidget.h"

#include <QPainter>
#include <QCheckBox>

MusicCheckBoxDelegate::MusicCheckBoxDelegate(QObject *parent)
    : QItemDelegate(parent)
{
    m_checkBox  = new QCheckBox;
    m_checkBox->setStyleSheet(MusicUIObject::MCheckBoxStyle01);
}

MusicCheckBoxDelegate::~MusicCheckBoxDelegate()
{
    delete m_checkBox;
}

QSize MusicCheckBoxDelegate::sizeHint(const QStyleOptionViewItem &option,
                                      const QModelIndex &) const
{
    QSize size = option.rect.size();
    size.setHeight(25);
    return size;
}

void MusicCheckBoxDelegate::paint(QPainter *painter,
                                  const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const
{
    painter->save();
    int minSize = qMin(option.rect.width(), option.rect.height());
    m_checkBox->resize(minSize, minSize);
    m_checkBox->setChecked( index.data(Qt::DisplayRole).toBool() );
    painter->translate(option.rect.width()/2, 0); // top left
    m_checkBox->render(painter, option.rect.topLeft(), QRegion(),
                       QWidget::DrawChildren);
    painter->restore();
}


MusicQueryTableWidget::MusicQueryTableWidget(QWidget *parent)
    : MusicAbstractTableWidget(parent)
{
    m_downLoadManager = new MusicDownLoadQueryThread(this);
    connect(m_downLoadManager, SIGNAL(clearAllItems()), SLOT(clearAllItems()));
    connect(m_downLoadManager, SIGNAL(creatSearchedItems(QString,QString,QString)),
                               SLOT(creatSearchedItems(QString,QString,QString)));

    m_checkBoxDelegate = new MusicCheckBoxDelegate(this);
    setItemDelegateForColumn(0, m_checkBoxDelegate);

    connect(this, SIGNAL(cellEntered(int,int)), SLOT(listCellEntered(int,int)));
    connect(this, SIGNAL(cellDoubleClicked(int,int)), SLOT(itemDoubleClicked(int,int)));
}

MusicQueryTableWidget::~MusicQueryTableWidget()
{
    delete m_checkBoxDelegate;
}

void MusicQueryTableWidget::listCellClicked(int row, int column)
{
    if(column == 0)
    {
        QTableWidgetItem *it = item(row, 0);
        bool status = it->data(Qt::DisplayRole).toBool();
        it->setData(Qt::DisplayRole, !status);
    }
    else
    {
        if(m_previousClickRow != -1)
        {
            item(m_previousClickRow, 0)->setData(Qt::DisplayRole, false);
        }
        m_previousClickRow = row;
        item(row, 0)->setData(Qt::DisplayRole, true);
    }
}

void MusicQueryTableWidget::setSelectedAllItems(bool all)
{
    for(int i=0; i<rowCount(); ++i)
    {
        item(i, 0)->setData(Qt::DisplayRole, all);
    }
}

void MusicQueryTableWidget::contextMenuEvent(QContextMenuEvent *)
{

}

void MusicQueryTableWidget::paintEvent(QPaintEvent *event)
{
    MusicAbstractTableWidget::paintEvent(event);
    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(QBrush(QColor(0,0,0)), 0.1, Qt::SolidLine));
    for(int i=0; i<rowCount(); ++i)
    {
        painter.drawLine(10, rowHeight(0)*(i + 1),
                         width() - 15, rowHeight(0)*(i + 1));
    }
}

MIntList MusicQueryTableWidget::getSelectedItems() const
{
    MIntList list;
    for(int i=0; i<rowCount(); ++i)
    {
        if(item(i, 0)->data(Qt::DisplayRole) == true)
        {
            list << i;
        }
    }
    return list;
}
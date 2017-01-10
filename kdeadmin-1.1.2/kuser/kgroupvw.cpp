#include "kgroupvw.moc"
#include "misc.h"

KGroupView::KGroupView(QWidget *parent, const char *name) : QWidget( parent, name )
{
  init();
  current = -1;
}

KGroupView::~KGroupView()
{
  delete m_Header;
  delete m_Groups;
}

void KGroupView::setAutoUpdate(bool state) {
  m_Groups->setAutoUpdate(state);
}

void KGroupView::clear() {
  m_Groups->clear();
}

void KGroupView::insertItem(KGroup *aku) {
  m_Groups->insertItem(aku);
}

int KGroupView::currentItem() {
  return (current);
}

void KGroupView::setCurrentItem(int item) {
  current = item;
  m_Groups->setCurrentItem(item);
}

void KGroupView::sortBy(int num) {
  m_Groups->sortBy(num);
}

void KGroupView::init()
{
  m_Header = new KHeader(this, "_gheader", 2, KHeader::Resizable|KHeader::Buttons );
  m_Header->setGeometry(2, 0, width(), 0 );

  m_Groups = new KGroupTable(this, "_gtable" );
  m_Groups->setGeometry(0, m_Header->height(), width(), height()-m_Header->height() );

  m_Header->setHeaderLabel(0, i18n("GID"));
  m_Header->setHeaderLabel(1, i18n("Group name"));

  connect(m_Groups, SIGNAL(highlighted(int,int)), SLOT(onHighlight(int,int)));
  connect(m_Groups, SIGNAL(selected(int,int)), SLOT(onSelect(int,int)));
  connect(m_Header, SIGNAL(selected(int)), SLOT(onHeaderClicked(int)));
  connect(m_Header, SIGNAL(sizeChanged(int,int)), m_Groups, SLOT(setColumnWidth(int,int)));

// This connection makes it jumpy and slow (but it works!)
  connect(m_Header, SIGNAL(sizeChanging(int,int)), m_Groups, SLOT(setColumnWidth(int,int)));

  connect(m_Groups, SIGNAL(hSliderMoved(int)), m_Header, SLOT(setOrigin(int)));

  m_Header->setHeaderSize(0, 60);
  m_Header->setHeaderSize(1, 300);
}

void KGroupView::repaint() {
  m_Groups->repaint();
  m_Header->repaint();
}

KGroup *KGroupView::getCurrentGroup() {
  return (((KGroupRow *)m_Groups->getRow(current))->getData());
}

void KGroupView::onSelect(int row, int)
{
  current = row;
  emit selected(row);
}

void KGroupView::onHighlight(int row, int)
{
  current = row;
  emit highlighted(row);
}

void KGroupView::onHeaderClicked(int num) {
  emit headerClicked(num);
}

void KGroupView::resizeEvent(QResizeEvent *rev)
{
  m_Header->resize(rev->size().width(), 0);
  m_Header->setHeaderSize(0, (rev->size().width()-20)*3/18);
  m_Header->setHeaderSize(1, (rev->size().width()-20)*15/18);

  m_Groups->setGeometry(0, m_Header->height(), rev->size().width(), rev->size().height()-m_Header->height());
}

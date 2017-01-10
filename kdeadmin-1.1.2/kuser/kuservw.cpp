#include "kuservw.moc"
#include "misc.h"

KUserView::KUserView(QWidget *parent, const char *name) : QWidget( parent, name )
{
  init();
  current = -1;
}

KUserView::~KUserView()
{
  delete m_Header;
  delete m_Users;
}

void KUserView::setAutoUpdate(bool state) {
  m_Users->setAutoUpdate(state);
}

void KUserView::clear() {
  m_Users->clear();
}

void KUserView::insertItem(KUser *aku) {
  m_Users->insertItem(aku);
}

int KUserView::currentItem() {
  return (current);
}

void KUserView::setCurrentItem(int item) {
  current = item;
  m_Users->setCurrentItem(item);
}

void KUserView::sortBy(int num) {
  m_Users->sortBy(num);
}

void KUserView::init()
{
  m_Header = new KHeader(this, "_header", 2, KHeader::Resizable|KHeader::Buttons );
  m_Header->setGeometry(2, 0, width(), 0 );

  m_Users = new KUserTable(this, "_table" );
  m_Users->setGeometry(0, m_Header->height(), width(), height()-m_Header->height() );

  m_Header->setHeaderLabel(0, i18n("User login"));
  m_Header->setHeaderLabel(1, i18n("Full Name"));

  connect(m_Users, SIGNAL(highlighted(int,int)), SLOT(onHighlight(int,int)));
  connect(m_Users, SIGNAL(selected(int,int)), SLOT(onSelect(int,int)));
  connect(m_Header, SIGNAL(selected(int)), SLOT(onHeaderClicked(int)));
  connect(m_Header, SIGNAL(sizeChanged(int,int)), m_Users, SLOT(setColumnWidth(int,int)));

// This connection makes it jumpy and slow (but it works!)
  connect(m_Header, SIGNAL(sizeChanging(int,int)), m_Users, SLOT(setColumnWidth(int,int)));

  connect(m_Users, SIGNAL(hSliderMoved(int)), m_Header, SLOT(setOrigin(int)));

  m_Header->setHeaderSize(0, 100);
  m_Header->setHeaderSize(1, 260);
}

void KUserView::repaint() {
  m_Users->repaint();
  m_Header->repaint();
}

KUser *KUserView::getCurrentUser() {
  return (((KUserRow *)m_Users->getRow(current))->getData());
}

void KUserView::onSelect(int row, int)
{
  current = row;
  emit selected(row);
}

void KUserView::onHighlight(int row, int)
{
  current = row;
  emit highlighted(row);
}

void KUserView::onHeaderClicked(int num) {
  emit headerClicked(num);
}

void KUserView::resizeEvent(QResizeEvent *rev)
{
  m_Header->resize(rev->size().width(), 0);
  m_Header->setHeaderSize(0, (rev->size().width()-20)*5/18);
  m_Header->setHeaderSize(1, (rev->size().width()-20)*13/18);

  m_Users->setGeometry(0, m_Header->height(), rev->size().width(), rev->size().height()-m_Header->height());
}

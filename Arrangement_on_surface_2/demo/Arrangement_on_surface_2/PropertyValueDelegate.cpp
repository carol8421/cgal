#include "ColorItemEditor.h"
#include "DeleteCurveModeItemEditor.h"
#include "PropertyValueDelegate.h"
#include "DeleteCurveMode.h"
#include <iostream>

PropertyValueDelegate::PropertyValueDelegate( QObject* parent ):
  QItemDelegate( parent )
{ 
  QItemEditorFactory* factory = new QItemEditorFactory;
  QItemEditorCreatorBase* creator =
    new QStandardItemEditorCreator< PositiveSpinBox >( );
  factory->registerEditor( QVariant::UInt, creator );
  this->setItemEditorFactory( factory );
}

QWidget* PropertyValueDelegate::
createEditor( QWidget* parent, const QStyleOptionViewItem& option,
              const QModelIndex& index ) const
{
  QWidget* editor;
  QVariant myData = index.data( Qt::UserRole );

  // check for data types we need to handle ourselves
  if ( qVariantCanConvert< QColor >( myData ) )
  {
    ColorItemEditor* colorEditor = new ColorItemEditor( parent );
    editor = colorEditor;

    QObject::connect( colorEditor, SIGNAL(confirmed()), this, SLOT(commit()));
  }
  else if ( qVariantCanConvert< DeleteCurveMode >( myData ) )
  {
    DeleteCurveModeItemEditor* modeEditor =
      new DeleteCurveModeItemEditor( parent );
    modeEditor->setMode( qVariantValue< DeleteCurveMode >( myData ) );
    editor = modeEditor;

    QObject::connect( modeEditor, SIGNAL( currentIndexChanged( int ) ), this,
                      SLOT( commit( ) ) );
  }
  else
  { // default handler
    editor = QItemDelegate::createEditor( parent, option, index );
  }

  return editor;
}

void PropertyValueDelegate::setModelData( QWidget* editor,
                                          QAbstractItemModel* model,
                                          const QModelIndex& index ) const
{
  ColorItemEditor* colorEditor = 0;
  DeleteCurveModeItemEditor* modeEditor = 0;
  if ( colorEditor = qobject_cast< ColorItemEditor* >( editor ) )
  {
    // std::cout << "set color model data" << std::endl;
    model->setData( index, colorEditor->color( ), Qt::DisplayRole );
    model->setData( index, colorEditor->color( ), Qt::DecorationRole );
    model->setData( index, QVariant::fromValue( colorEditor->color( ) ),
                    Qt::UserRole );
  }
  else if ( modeEditor = qobject_cast< DeleteCurveModeItemEditor* >( editor ) )
  {
    model->setData( index, DeleteCurveMode::ToString( modeEditor->mode( ) ),
                    Qt::DisplayRole );
    model->setData( index, QVariant::fromValue( modeEditor->mode( ) ),
                    Qt::UserRole );
  }
  else
  {
    QItemDelegate::setModelData( editor, model, index );
  }
}

bool PropertyValueDelegate::eventFilter( QObject *object, QEvent *event )
{
  QWidget *editor = qobject_cast<QWidget*>(object);
  ColorItemEditor* colorEditor;
  DeleteCurveModeItemEditor* modeEditor;
  if (event->type() == QEvent::FocusOut ||
      (event->type() == QEvent::Hide && editor->isWindow()))
  {
    if ( colorEditor = qobject_cast< ColorItemEditor* >( editor ) )
    {
      return false;
    }
    if ( modeEditor = qobject_cast< DeleteCurveModeItemEditor* >( editor ) )
    {
      return false;
    }
  }
  return QItemDelegate::eventFilter( object, event );
}

void PropertyValueDelegate::commit( )
{
  // std::cout << "commit selection" << std::endl;
  QWidget* editor = qobject_cast< QWidget* >( sender( ) );
  if ( editor )
  {
    emit( commitData( editor ) );
    emit( closeEditor( editor ) );
  }
}

PositiveSpinBox::PositiveSpinBox( QWidget* parent ):
  QSpinBox( parent )
{
  this->setMinimum( 1 );
}

void PositiveSpinBox::setValue( unsigned int val )
{
  QSpinBox::setValue( val );
}

unsigned int PositiveSpinBox::value( ) const
{
  return QSpinBox::value( );
}

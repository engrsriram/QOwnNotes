#include "trashdialog.h"
#include "ui_trashdialog.h"
#include <QSettings>
#include <QDebug>
#include <QScriptValueIterator>
#include <QPushButton>

TrashDialog::TrashDialog(QScriptValue notes, MainWindow *mainWindow, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TrashDialog)
{
    this->mainWindow = mainWindow;
    ui->setupUi(this);
    setupMainSplitter();

    QPushButton *button;
    ui->buttonBox->clear();

    QIcon restoreIcon;
    restoreIcon.setThemeName( "view-restore" );

    QIcon cancelIcon;
    cancelIcon.setThemeName( "application-exit" );

    button = new QPushButton( tr( "&Restore selected note" ) );
    button->setProperty( "ActionRole", Restore );
    button->setDefault( false );
    button->setIcon( restoreIcon );
    ui->buttonBox->addButton( button, QDialogButtonBox::ActionRole );

    button = new QPushButton( tr( "&Cancel" ) );
    button->setProperty( "ActionRole", Cancel );
    button->setIcon( cancelIcon );
    button->setDefault( true );
    ui->buttonBox->addButton( button, QDialogButtonBox::ActionRole );

    connect( this->ui->buttonBox, SIGNAL( clicked( QAbstractButton* ) ), SLOT( dialogButtonClicked( QAbstractButton* ) ) );
    connect( this, SIGNAL( finished(int) ), this, SLOT( storeSettings() ) );

    QString itemName;
    QString dateString;
    QString data;
    ui->trashListWidget->clear();
    dataList = new QStringList();

    // init the iterator for the verions
    QScriptValueIterator notesIterator( notes );

    // iterate over the trashs
    while ( notesIterator.hasNext() ) {
        notesIterator.next();
        qDebug() << notesIterator.value().property( "timestamp" ).toString() << " - " << notesIterator.value().property( "dateString" ).toString() << " - " << notesIterator.value().property( "noteName" ).toString();

        itemName = notesIterator.value().property( "noteName" ).toString();
        dateString = notesIterator.value().property( "dateString" ).toString();
        data = notesIterator.value().property( "data" ).toString();

        if ( itemName == "" ) {
            continue;
        }

        QListWidgetItem *item = new QListWidgetItem();
        item->setText( itemName );
        item->setToolTip( dateString );
        ui->trashListWidget->addItem( item );
        dataList->append( data );
    }

    ui->trashListWidget->setCurrentRow( 0 );
    ui->noteBrowser->setText( dataList->at( 0 ) );
}

void TrashDialog::setupMainSplitter()
{
    this->trashSplitter = new QSplitter;

    this->trashSplitter->addWidget(ui->trashListWidget);
    this->trashSplitter->addWidget(ui->noteBrowser);

    // restore splitter sizes
    QSettings settings;
    QByteArray state = settings.value( "trashSplitterSizes" ).toByteArray();
    this->trashSplitter->restoreState( state );

    this->ui->gridLayout->layout()->addWidget( this->trashSplitter );
    this->ui->gridLayout->layout()->addWidget( ui->buttonBox );
}

void TrashDialog::storeSettings()
{
    // store the splitter sizes
    QSettings settings;
    settings.setValue( "trashSplitterSizes", this->trashSplitter->saveState() );
}

TrashDialog::~TrashDialog()
{
    delete ui;
}

void TrashDialog::on_trashListWidget_currentRowChanged(int currentRow)
{
    ui->noteBrowser->setText( dataList->value( currentRow ) );
}

void TrashDialog::dialogButtonClicked( QAbstractButton *button )
{
    int actionRole = button->property("ActionRole").toInt();

    if ( actionRole == Restore )
    {
        QString name = ui->trashListWidget->currentItem()->text();
        QString text = dataList->value( ui->trashListWidget->currentRow() );
        mainWindow->createNewNote( name, text );
    }

    this->close();
}
#include "labeldialog.h"
#include "ui_labelDialog.h"
#include <QCompleter>
#include <QListWidget>
#include <QListWidgetItem>
#include <QIcon>
#include <QLineEdit>
#include <QKeyEvent>
#include <QtDebug>

LabelDialog::LabelDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LabelDialog)
{
    ui->setupUi(this);

//    for (auto label:labelManager.getLabels()){
//        QListWidgetItem *item = new QListWidgetItem(ColorUtils::iconFromColor(label.color),label.label, ui->listWidget);
//        ui->listWidget->addItem(item);
//    }

    ui->lineEdit->setLabelListWidget(ui->listWidget);
}

LabelDialog::~LabelDialog()
{
    delete ui;
}

QString LabelDialog::getLabel() const
{
    return ui->lineEdit->text();
}

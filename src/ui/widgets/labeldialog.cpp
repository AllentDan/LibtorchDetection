#include "labeldialog.h"
#include "ui_labelDialog.h"
#include <QCompleter>
#include <QListWidget>
#include <QListWidgetItem>
#include <QIcon>
#include <QLineEdit>
#include <QKeyEvent>
#include <QtDebug>

#include "../managers/colormanager.h"

LabelDialog::LabelDialog(const LabelManager& label_manager, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LabelDialog)
{
    ui->setupUi(this);

    for (auto label:label_manager.getLabels()){
        QListWidgetItem *item = new QListWidgetItem(ColorManager::iconFromColor(label_manager[label]),label, ui->listWidget);
        ui->listWidget->addItem(item);
    }

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

#include "labeltool.h"
#include "ui_labeltool.h"

#include <QMouseEvent>
#include <QDebug>
#include <QScrollArea>

labelTool::labelTool(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::labelTool)
{
    ui->setupUi(this);

    canvas = new Canvas(&label_manager, &annotation_manager, ui->scrollArea);
//    canvas->setVisible(true);
    canvas->setEnabled(true);
    ui->scrollArea->setAlignment(Qt::AlignCenter);
    ui->scrollArea->setWidget(canvas);
    ui->scrollArea->setWidgetResizable(true);
    initUI();
}

labelTool::~labelTool()
{
    delete ui;
    delete canvas;
}

void labelTool::readFiles(){
    QStringList tmp_file_names = QFileDialog::getOpenFileNames(this,
        tr("Open image files"), "D:\\AllentFiles\\data\\dataset\\images", tr("Image Files (*.jpg *.png *.bmp)"));
    if (tmp_file_names.size() == 0) return;
    this->file_names += tmp_file_names;
    ui->FileList->addItems(tmp_file_names);
    if(this->cur_file_idx == -1) this->cur_file_idx = 0;
    this->canvas->loadPixmap(this->file_names[this->cur_file_idx]);
    adjustFitWindow();
    return;
}

void labelTool::readDir(){
    QString dirName = QFileDialog::getExistingDirectory(this, "open a dir", "D:\\AllentFiles\\data\\dataset");
    if (!dirName.isNull() && !dirName.isEmpty()){
        QDir dir(dirName);
        QStringList images = dir.entryList(QStringList() << "*.jpg"<<"*.png"<<"*.bmp", QDir::Files);
        images.sort();
        if (!dirName.endsWith('/')) dirName+="/";
        QStringList tmp;
        for (auto &image: images){
            tmp.push_back(dirName+image);
        }
        this->file_names += tmp;
        ui->FileList->addItems(tmp);
        if(this->cur_file_idx == -1) this->cur_file_idx = 0;
        this->canvas->loadPixmap(this->file_names[this->cur_file_idx]);
        adjustFitWindow();
    }
    return;
}

void labelTool::file_list_row_clicked(int num){
    this->cur_file_idx = num;
    this->canvas->loadPixmap(this->file_names[this->cur_file_idx]);
    adjustFitWindow();
}

void labelTool::provideFileContextMenu(const QPoint& pos){
    QPoint globalPos = ui->FileList->mapToGlobal(pos);
    QModelIndex modelIdx = ui->FileList->indexAt(pos);
    if (!modelIdx.isValid()) return;
    int row = modelIdx.row();

    QMenu submenu;
    submenu.addAction("Change Color");
    submenu.addAction("Delete");
    QAction* rightClickItem = submenu.exec(globalPos);
    if (rightClickItem){
        if (rightClickItem->text().contains("Delete")){
            delete ui->FileList->takeItem(row);
            if (this->file_names.size()-1 != row) this->cur_file_idx--;
            this->file_names.removeAt(row);
            this->cur_file_idx = std::min(this->cur_file_idx, this->file_names.size()-1);
            this->canvas->loadPixmap(this->file_names[this->cur_file_idx]);
            adjustFitWindow();
        }
    }
    return;
}

qreal labelTool::scaleFitWindow() const
{
    int w1 = ui->scrollArea->width() - 2; // -2 So that no scrollbars are generated.
    int h1 = ui->scrollArea->height() - 2;
    qreal a1 = static_cast<qreal>(w1)/h1;
    int w2 = canvas->sizeUnscaled().width();
    int h2 = canvas->sizeUnscaled().height();
    qreal a2 = static_cast<qreal>(w2)/h2;
    return a2>=a1 ? static_cast<qreal>(w1)/w2 : static_cast<qreal>(h1)/h2;
}

void labelTool::adjustFitWindow()
{
    canvas->setScale(scaleFitWindow());
}


void labelTool::reportMouseMoved(QPoint pos){
    ui->mousePosLabel->setText("("+ QString::number(pos.x())+","+QString::number(pos.y())+")");
}

QString labelTool::getCurrentLabel() const
{
    auto selectedLabels = ui->LabelList->selectedItems();
    if (selectedLabels.length()==1){
        return selectedLabels[0]->text();
    }else if (selectedLabels.length()==0){
        return "";
    }else {
        throw "selected mutiple label in the list";
    }
}

void labelTool::newLabelRequest(QString newLabel)
{
    if (newLabel.isNull() || newLabel.isEmpty()) return;
    if (!label_manager.hasLabel(newLabel)){
        QColor newColor = ColorManager::randomColor();
        label_manager.addLabel(newLabel, newColor);
    }
}

void labelTool::getNewRect(QRect rect)
{
    QString label = _labelRequest();
    if (label=="") return;
    annotation_manager.add(detObj({label, rect.left(), rect.top(), rect.right(), rect.bottom()}));
}


QString labelTool::_labelRequest()
{
    QString curLabel = getCurrentLabel();
    if (curLabel==""){
        LabelDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted) {
            QString newLabel = dialog.getLabel();
            // newLabel 也有可能是 "" ，说明dialog被点击了取消
            newLabelRequest(newLabel);
            return newLabel;
        }else {
            return "";
        }
    }else{
        return curLabel;
    }
}

void labelTool::setAnntationDir(){
    QString dirName = QFileDialog::getExistingDirectory(this, "open a dir", "D:\\AllentFiles\\data\\dataset");
    if (!dirName.isNull()){
        this->annotationDir = dirName;
        canvas->annotationDir = dirName;
    }
}

void labelTool::loadLabels(){
    QString label_path = QFileDialog::getOpenFileName(this,
                                                   tr("Open labels"),
                                                   "D:\\AllentFiles\\data\\dataset\\labels.txt",
                                                   tr("Text Files (*.txt)"));
    if (QFile::exists(label_path)){
        canvas->label_manager->fromTXT(label_path);
    }
}

void labelTool::initUI(){
    connect(ui->actionOpenFiles, SIGNAL(triggered()), this, SLOT(readFiles()));
    connect(ui->actionOpenDir, SIGNAL(triggered()), this, SLOT(readDir()));
    ui->FileList->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(ui->FileList, SIGNAL(currentRowChanged(int)), this, SLOT(file_list_row_clicked(int)));
    connect(ui->FileList, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(provideFileContextMenu(const QPoint&)));
    connect(this->canvas, &Canvas::newRectangleAnnotated, this, &labelTool::getNewRect);
    connect(this->canvas, &Canvas::mouseMoved, this, &labelTool::reportMouseMoved);

    ui->actionNextImage->setEnabled(false);
    ui->actionPreviousImage->setEnabled(false);
    connect(ui->actionFitWindow, &QAction::triggered, this, &labelTool::adjustFitWindow);
    connect(ui->actionZoomIn, &QAction::triggered, [this](){ this->canvas->setScale(canvas->getScale()*1.1); });
    connect(ui->actionZoomOut, &QAction::triggered, [this](){ this->canvas->setScale(canvas->getScale()*0.9); });
    connect(ui->actionSetAnnotationDir, &QAction::triggered, this, &labelTool::setAnntationDir);
    connect(ui->actionLoadLabels, &QAction::triggered, this, &labelTool::loadLabels);
}

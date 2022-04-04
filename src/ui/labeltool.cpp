#include "labeltool.h"
#include "ui_labeltool.h"

#include <QMouseEvent>
#include <QDebug>
#include <QScrollArea>
#include <QColorDialog>

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

void labelTool::_readDir(QString dirName){
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

void labelTool::readDir(){
    QString dirName = QFileDialog::getExistingDirectory(this, "open a dir", "D:\\AllentFiles\\data\\dataset");
    _readDir(dirName);
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

void labelTool::provideAnnotationContextMenu(const QPoint &pos)
{
    QPoint globalPos = ui->AnnotationList->mapToGlobal(pos);
    QModelIndex modelIdx = ui->AnnotationList->indexAt(pos);
    if (!modelIdx.isValid())return;
    int row = modelIdx.row();

    QMenu submenu;
    submenu.addAction("Delete");
    QAction* rightClickItem = submenu.exec(globalPos);
    if (rightClickItem){
        if (rightClickItem->text().contains("Delete")){
            delete ui->AnnotationList->takeItem(row);
            this->annotation_manager.objects.removeAt(row);
            update();
        }
    }
    return;
}

void labelTool::provideLabelContextMenu(const QPoint &pos)
{
    QPoint globalPos = ui->LabelList->mapToGlobal(pos);
    QModelIndex modelIdx = ui->LabelList->indexAt(pos);
    if (!modelIdx.isValid())return;
    int row = modelIdx.row();
    auto item = ui->LabelList->item(row);

    QMenu submenu;
    submenu.addAction("Change Color");
    submenu.addAction("Delete");
    QAction* rightClickItem = submenu.exec(globalPos);
    if (rightClickItem){
        if (rightClickItem->text().contains("Delete")){
            delete ui->LabelList->takeItem(row);
            this->label_manager.removeLabel(item->text());
            update();
        }else if (rightClickItem->text().contains("Change Color")){
            QColor color = QColorDialog::getColor(Qt::white, this, "Choose Color");
            if (color.isValid()){
                label_manager.setColor(item->text(),color);
                ui->LabelList->changeIconColorByIdx(row, color);
                reloadAnnotations();
            }
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
        QListWidgetItem *item = new QListWidgetItem(ColorManager::iconFromColor(newColor), newLabel);
        ui->LabelList->addItem(item);
    }
}

void labelTool::getNewRect(QRect rect)
{
    QString label = _labelRequest();
    if (label=="") return;
    detObj obj = detObj({label, rect.left(), rect.top(), rect.right(), rect.bottom()});
    annotation_manager.add(obj);
    QColor color = label_manager[label];
    QListWidgetItem *item = new QListWidgetItem(ColorManager::iconFromColor(color),obj.toString());
    ui->AnnotationList->addItem(item);
}


QString labelTool::_labelRequest()
{
    QString curLabel = getCurrentLabel();
    if (curLabel==""){
        LabelDialog dialog(*canvas->label_manager, this);
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
        canvas->annotation_manager->annotation_dir = dirName;
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

void labelTool::saveAnnotation(){
    annotation_manager.toXml();
}

void labelTool::loadDatasets(){
    QString dirName = QFileDialog::getExistingDirectory(this, "open a dir", "D:\\AllentFiles\\data\\dataset");
    if (!(dirName +"/labels").isNull()){
        this->annotationDir = dirName +"/labels";
        canvas->annotationDir = dirName +"/labels";
        canvas->annotation_manager->annotation_dir = dirName +"/labels";
    }
    if(QFile::exists(dirName + "/labels.txt")){
        canvas->label_manager->fromTXT(dirName + "/labels.txt");
        QStringList labels = canvas->label_manager->getLabels();
        for(auto label: labels) {
            QColor color = (*canvas->label_manager)[label];
            QListWidgetItem *item = new QListWidgetItem(ColorManager::iconFromColor(color), label);
            ui->LabelList->addItem(item);
        }
    }
    _readDir(dirName +"/images");
}

void labelTool::saveDatasets(){
//    QString dirName = QFileDialog::getExistingDirectory(this, "open a dir", "D:\\AllentFiles\\data\\dataset");
//    if (!(dirName +"/labels").isNull()){
//        this->annotationDir = dirName +"/labels";
//        canvas->annotationDir = dirName +"/labels";
//        canvas->annotation_manager->annotation_dir = dirName +"/labels";
//    }
//    if(QFile::exists(dirName + "/labels.txt")){
//        canvas->label_manager->fromTXT(dirName + "/labels.txt");
//    }
//    _readDir(dirName +"/images");
}

void labelTool::reloadAnnotations(){
    ui->AnnotationList->clear();
    for (detObj obj : annotation_manager.objects) {
        QColor color = label_manager[obj.label];
        QListWidgetItem *item = new QListWidgetItem(ColorManager::iconFromColor(color),obj.toString());
        ui->AnnotationList->addItem(item);
    }
}


void labelTool::initUI(){
    connect(ui->actionOpenFiles, SIGNAL(triggered()), this, SLOT(readFiles()));
    connect(ui->actionOpenDir, SIGNAL(triggered()), this, SLOT(readDir()));
    ui->FileList->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(ui->FileList, SIGNAL(currentRowChanged(int)), this, SLOT(file_list_row_clicked(int)));
    connect(ui->FileList, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(provideFileContextMenu(const QPoint&)));

    connect(ui->AnnotationList, &QListWidget::itemSelectionChanged, [this](){
        auto items = ui->AnnotationList->selectedItems();
        if (items.length()==0){//actually not working, has to inherit mousePress event in its custom widget
            this->canvas->mode = DRAW;
            this->canvas->selectedIdx = -1;
        }else{
            int idx = ui->AnnotationList->row(items[0]);
            this->canvas->mode = SELECT;
            this->canvas->selectedIdx = idx;
        }
        this->canvas->update();
    });
    ui->AnnotationList->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(ui->AnnotationList, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(provideAnnotationContextMenu(const QPoint&)));
    ui->LabelList->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    connect(ui->LabelList, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(provideLabelContextMenu(const QPoint&)));

    connect(this->canvas, &Canvas::newRectangleAnnotated, this, &labelTool::getNewRect);
    connect(this->canvas, &Canvas::mouseMoved, this, &labelTool::reportMouseMoved);
    connect(this->canvas, &Canvas::reloadAnnotations, this, &labelTool::reloadAnnotations);

    ui->actionNextImage->setEnabled(false);
    ui->actionPreviousImage->setEnabled(false);
    connect(ui->actionFitWindow, &QAction::triggered, this, &labelTool::adjustFitWindow);
    connect(ui->actionZoomIn, &QAction::triggered, [this](){ this->canvas->setScale(canvas->getScale()*1.1); });
    connect(ui->actionZoomOut, &QAction::triggered, [this](){ this->canvas->setScale(canvas->getScale()*0.9); });
    connect(ui->actionSetAnnotationDir, &QAction::triggered, this, &labelTool::setAnntationDir);
    connect(ui->actionLoadLabels, &QAction::triggered, this, &labelTool::loadLabels);
    connect(ui->actionSave, &QAction::triggered, this, &labelTool::saveAnnotation);
    connect(ui->actionLoadDatasets, &QAction::triggered, this, &labelTool::loadDatasets);
    connect(ui->actionSaveDataset, &QAction::triggered, this, &labelTool::saveDatasets);

    connect(ui->actionTraining, &QAction::triggered, [this](){
        ui->centralWidget->hide();
        ui->AnnotationDock->hide();
        ui->LabelDock->hide();
        ui->FileDock->hide();
        ui->toolBar->hide();
    });
    connect(ui->actionLabelTool, &QAction::triggered, [this](){
        ui->centralWidget->show();
        ui->AnnotationDock->show();
        ui->LabelDock->show();
        ui->FileDock->show();
        ui->toolBar->show();
    });
}

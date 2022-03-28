#ifndef LABELTOOL_H
#define LABELTOOL_H

#include <QMainWindow>
#include <QFileDialog>
#include "canvas.h"

QT_BEGIN_NAMESPACE
namespace Ui { class labelTool; }
QT_END_NAMESPACE

class labelTool : public QMainWindow
{
    Q_OBJECT

public:
    labelTool(QWidget *parent = nullptr);
    ~labelTool();
    qreal scaleFitWindow() const;
    QString getCurrentLabel() const;

protected:
    QStringList file_names;
    int cur_file_idx = -1;

private:
    Ui::labelTool *ui;
    Canvas *canvas;
    LabelManager label_manager;
    AnnotationManager annotation_manager;
    QString annotationDir = "";
    void initUI();
    QString _labelRequest();

private slots:
    void readFiles();
    void readDir();
    void setAnntationDir();
    void loadLabels();
    void file_list_row_clicked(int num);
    void getNewRect(QRect rect);
    void newLabelRequest(QString newLabel);
    void adjustFitWindow();

    void provideFileContextMenu(const QPoint& pos);
    void reportMouseMoved(QPoint pos);

};
#endif // LABELTOOL_H

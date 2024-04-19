#ifndef SAVEWIN_H
#define SAVEWIN_H

#include <QDialog>
#include <QGraphicsScene>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QImage>
#include <QString>
#include <QProgressBar>

class SaveWin : public QDialog
{
    Q_OBJECT

public:
    explicit SaveWin(QGraphicsScene *scene,QWidget *parent=nullptr);
    ~SaveWin();

    QString getSelectedFilePath() const;
    QImage getRenderedImage() const;

private slots:
    void onSaveClicked();
    void onCancelClicked();

private:
    QGraphicsScene *scene;
    QComboBox *formatComboBox;
    QSpinBox *widthSpinBox;
    QSpinBox *heightSpinBox;
    QSpinBox *qualitySpinBox;
    QCheckBox *antialiasingCheckBox;
    QPushButton *btnSave;
    QPushButton *btnCancel;
    QProgressBar *progressBar;

    QString selectedFilePath;
    QImage renderedImage;

    void setupUI();
    void renderScene();
};

#endif // SAVEWIN_H

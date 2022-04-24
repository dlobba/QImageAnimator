#ifndef SCREENAREASELECTION_H
#define SCREENAREASELECTION_H

#include <QDialog>

class QMouseEvent;
class QRubberBand;

class ScreenAreaSelection : public QDialog
{
    Q_OBJECT
public:
    explicit ScreenAreaSelection(QWidget *aParent = nullptr);

private slots:
    void mousePressEvent(QMouseEvent *) override;
    void mouseMoveEvent(QMouseEvent *) override;
    void mouseReleaseEvent(QMouseEvent *) override;

signals:
    void ScreenAreaSelected(const QRect &);

private:
    QRubberBand *mRubberBand;
    QPoint mOrigin;
};

#endif // SCREENAREASELECTION_H

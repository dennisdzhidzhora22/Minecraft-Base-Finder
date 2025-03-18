// zoomablegraphicsview.h
#ifndef ZOOMABLEGRAPHICSVIEW_H
#define ZOOMABLEGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>

class ZoomableGraphicsView : public QGraphicsView {
    Q_OBJECT
public:
    explicit ZoomableGraphicsView(QWidget* parent = nullptr)
        : QGraphicsView(parent) {
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    }

protected:
    void wheelEvent(QWheelEvent* event) override {
        const double scaleFactor = 1.15; // Adjust zoom sensitivity
        if (event->angleDelta().y() > 0)
            scale(scaleFactor, scaleFactor);
        else
            scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
};

#endif // ZOOMABLEGRAPHICSVIEW_H
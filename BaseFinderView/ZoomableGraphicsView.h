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
        scale(100.0, 100.0);

        setMouseTracking(true);
    }

signals:
    void mouseMoved(int chunkX, int chunkZ);

protected:
    void wheelEvent(QWheelEvent* event) override {
        const double scaleFactor = 1.15; // Adjust zoom sensitivity
        if (event->angleDelta().y() > 0)
            scale(scaleFactor, scaleFactor);
        else
            scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }

    void mouseMoveEvent(QMouseEvent* event) override {
        QGraphicsView::mouseMoveEvent(event);

        QPointF scenePos = mapToScene(event->pos());
        emit mouseMoved(static_cast<int>(floor(scenePos.x())), static_cast<int>(floor(scenePos.y())));
    }
};

#endif // ZOOMABLEGRAPHICSVIEW_H
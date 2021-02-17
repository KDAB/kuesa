#ifndef VIEWPORTMANAGER_H
#define VIEWPORTMANAGER_H

#include <QObject>
#include <Kuesa/ForwardRenderer>
#include "abstractscreencontroller.h"

class ViewportManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Kuesa::ForwardRenderer *frameGraph READ frameGraph WRITE setFrameGraph NOTIFY frameGraphChanged)
    Q_PROPERTY(AbstractScreenController *controller READ controller WRITE setController NOTIFY controllerChanged)
public:
    explicit ViewportManager(QObject *parent = nullptr);
    ~ViewportManager();

    Kuesa::ForwardRenderer *frameGraph() const;
    AbstractScreenController *controller() const;

    void setFrameGraph(Kuesa::ForwardRenderer *frameGraph);
    void setController(AbstractScreenController *controller);

signals:
    void controllerChanged();
    void frameGraphChanged();

private:
    Kuesa::ForwardRenderer *m_frameGraph = nullptr;
    AbstractScreenController *m_controller = nullptr;

    void synchronizeViews();
};

#endif // VIEWPORTMANAGER_H

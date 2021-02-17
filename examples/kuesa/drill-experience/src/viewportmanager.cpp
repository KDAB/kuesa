#include "viewportmanager.h"

ViewportManager::ViewportManager(QObject *parent)
    : QObject(parent)
{
}

ViewportManager::~ViewportManager()
{
}

Kuesa::ForwardRenderer *ViewportManager::frameGraph() const
{
    return m_frameGraph;
}

AbstractScreenController *ViewportManager::controller() const
{
    return m_controller;
}

void ViewportManager::setFrameGraph(Kuesa::ForwardRenderer *frameGraph)
{
    if (frameGraph == m_frameGraph)
        return;
    m_frameGraph = frameGraph;
    emit frameGraphChanged();

    synchronizeViews();
}

void ViewportManager::setController(AbstractScreenController *controller)
{
    if (controller == m_controller)
        return;

    if (m_controller)
        QObject::disconnect(m_controller, &AbstractScreenController::viewsChanged,
                            this, &ViewportManager::synchronizeViews);

    m_controller = controller;
    emit controllerChanged();

    if (m_controller)
        QObject::connect(m_controller, &AbstractScreenController::viewsChanged,
                         this, &ViewportManager::synchronizeViews);

    synchronizeViews();
}

void ViewportManager::synchronizeViews()
{
    if (m_frameGraph == nullptr)
        return;
    {
        const std::vector<Kuesa::View *> views = m_frameGraph->views();
        for (Kuesa::View *v : views)
            m_frameGraph->removeView(v);
    }
    if (m_controller) {
        const std::vector<Kuesa::View *> views = m_controller->views();
        for (Kuesa::View *v : views) {
            if (!v->camera())
                v->setCamera(m_frameGraph->camera());
            m_frameGraph->addView(v);
        }
    }
}

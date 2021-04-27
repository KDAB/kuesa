#include "stencilhelper.h"

#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QStencilMask>
#include <Qt3DRender/QStencilTest>
#include <Qt3DRender/QStencilTestArguments>
#include <Qt3DRender/QStencilOperation>
#include <Qt3DRender/QStencilOperationArguments>
namespace {

template<typename ComponentType>
ComponentType *componentFromEntity(Qt3DCore::QEntity *e)
{
    const auto cmps = e->componentsOfType<ComponentType>();
    return cmps.size() > 0 ? cmps.first() : nullptr;
}
}

StencilHelper::StencilHelper(QObject *parent)
    : QObject(parent)
    , m_entity(nullptr)
{
}

Qt3DCore::QEntity *StencilHelper::entity() const
{
    return m_entity;
}

void StencilHelper::setEntity(Qt3DCore::QEntity *entity)
{
    if (entity != m_entity)
    {
        m_entity = entity;

        // Search for all the QMaterials that exist under this entity
        std::vector<Qt3DRender::QMaterial *> materials;
        const auto entities = m_entity->findChildren<Qt3DCore::QEntity*>();

        Qt3DRender::QStencilTest *stencilTest = new Qt3DRender::QStencilTest(m_entity);
        stencilTest->front()->setStencilFunction(Qt3DRender::QStencilTestArguments::Always);
        stencilTest->back()->setStencilFunction(Qt3DRender::QStencilTestArguments::Always);
        Qt3DRender::QStencilOperation *stencilOperation = new Qt3DRender::QStencilOperation(m_entity);
        stencilOperation->front()->setAllTestsPassOperation(Qt3DRender::QStencilOperationArguments::Increment);
        stencilOperation->back()->setAllTestsPassOperation(Qt3DRender::QStencilOperationArguments::Increment);

        for (auto *childEntity : entities) {
            auto material = componentFromEntity<Qt3DRender::QMaterial>(childEntity);
            if (material) {
                auto *effect = qobject_cast<Qt3DRender::QEffect*>(material->effect()->metaObject()->newInstance());
                material->setEffect(effect);
                const auto &techniques = effect->techniques();
                for (auto *technique : techniques) {
                    const auto &renderPasses = technique->renderPasses();
                    for (auto *renderPass : renderPasses) {
                        renderPass->addRenderState(stencilTest);
                        renderPass->addRenderState(stencilOperation);
                    }
                }
            }
        }
        emit entityChanged();
    }
}

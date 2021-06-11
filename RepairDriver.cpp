#include "RepairDriver.h"
#include <QGraphicsDropShadowEffect>

RepairDriver::RepairDriver(QWidget *parent)
    : commonWidget(parent)
{
    ui.setupUi(this);
    QGraphicsDropShadowEffect* pGraphics = new QGraphicsDropShadowEffect(this);
    pGraphics->setOffset(0, 0);
    pGraphics->setColor(Qt::gray);
    pGraphics->setBlurRadius(10);
    ui.widget->setGraphicsEffect(pGraphics);

    connect(ui.closeBtn, &QPushButton::clicked, [this]() {
        close();
    });
}

RepairDriver::~RepairDriver()
{
}
